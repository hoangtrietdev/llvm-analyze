//===- HFLoopAnalysis.cpp - Loop-level dependence oracle for Hybrid-Flow --===//
//
// Hybrid-Flow v2 static oracle ("HF-Static").
//
// For every natural loop in a function this pass decides one of three
// verdicts, following the three-valued oracle Safe(L) used in the paper:
//
//   PARALLEL  (Safe = 1)  no loop-carried dependence can exist; the loop can
//                         be run as `omp parallel for` with the reported
//                         reduction clauses.
//   SERIAL    (Safe = 0)  a loop-carried dependence is *proven* (consistent
//                         dependence with a known non-'=' direction, a scalar
//                         recurrence that is neither an induction nor a
//                         reduction, an early exit, or an order-dependent
//                         library call such as printf/rand).
//   UNKNOWN   (Safe = ⊥)  nothing is proven either way (may-alias pointers,
//                         non-affine/indirect subscripts, opaque calls,
//                         non-computable trip counts, ...).
//
// The analysis uses LoopInfo, ScalarEvolution, LLVM DependenceAnalysis,
// InductionDescriptor / RecurrenceDescriptor and alias analysis.  One JSON
// object per loop is appended to the file named by $HF_STATIC_OUT (default
// hf_static.jsonl).  Loops whose start location is an inlined copy are
// skipped so that every source loop is reported once.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/IVDescriptors.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/MemoryLocation.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Config/llvm-config.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/JSON.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <string>

using namespace llvm;

namespace {

enum class Verdict { Parallel, Serial, Unknown };

const char *verdictName(Verdict V) {
  switch (V) {
  case Verdict::Parallel: return "PARALLEL";
  case Verdict::Serial:   return "SERIAL";
  case Verdict::Unknown:  return "UNKNOWN";
  }
  return "UNKNOWN";
}

std::string recurKindName(RecurKind K) {
  if (K == RecurKind::SMin || K == RecurKind::UMin || K == RecurKind::FMin)
    return "min";
  if (K == RecurKind::SMax || K == RecurKind::UMax || K == RecurKind::FMax)
    return "max";
  if (RecurrenceDescriptor::isAnyOfRecurrenceKind(K))
    return "anyof";
#if LLVM_VERSION_MAJOR >= 20
  if (RecurrenceDescriptor::isFindLastIVRecurrenceKind(K))
    return "lastiv";
#endif
  if (RecurrenceDescriptor::isMinMaxRecurrenceKind(K))
    return "minmax";
  switch (RecurrenceDescriptor::getOpcode(K)) {
  case Instruction::Add: case Instruction::FAdd: return "+";
  case Instruction::Mul: case Instruction::FMul: return "*";
  case Instruction::And: return "&";
  case Instruction::Or:  return "|";
  case Instruction::Xor: return "^";
  default: return "other";
  }
}

// Library calls whose effect depends on the order in which they execute.
const StringSet<> &orderDependentCalls() {
  static StringSet<> S = {"printf", "fprintf", "puts", "putchar", "fputs",
                          "fwrite", "fputc", "putc", "scanf", "fscanf",
                          "getchar", "fgets", "fread", "rand", "random",
                          "srand", "srandom", "drand48", "lrand48",
                          "\x01_printf", "\x01_puts", "\x01_fputs",
                          "\x01_fwrite"};
  return S;
}

// Calls that are thread-safe and carry no ordering constraint: allocation,
// and C99 <math.h> functions whose only side effect is setting the
// thread-local errno.
const StringSet<> &threadSafeCalls() {
  static StringSet<> S = {
      "malloc", "calloc", "free", "aligned_alloc", "_Znwm", "_Znam", "_ZdlPv",
      "_ZdaPv", "_ZdlPvm", "_ZdaPvm",
      "sqrt", "sqrtf", "exp", "expf", "exp2", "exp2f", "expm1", "log", "logf",
      "log2", "log2f", "log10", "log10f", "log1p", "pow", "powf", "sin", "sinf",
      "cos", "cosf", "tan", "tanf", "asin", "acos", "atan", "atan2", "atan2f",
      "sinh", "cosh", "tanh", "tanhf", "erf", "erff", "erfc", "erfcf", "cbrt",
      "hypot", "fmod", "fmodf", "floor", "floorf", "ceil", "ceilf", "round",
      "lround", "fabs", "fabsf", "fmin", "fmax", "fma"};
  return S;
}

bool isIgnorableIntrinsic(const Instruction &I) {
  if (auto *II = dyn_cast<IntrinsicInst>(&I)) {
    if (II->isAssumeLikeIntrinsic() || isa<DbgInfoIntrinsic>(II))
      return true;
    switch (II->getIntrinsicID()) {
    case Intrinsic::lifetime_start:
    case Intrinsic::lifetime_end:
    case Intrinsic::experimental_noalias_scope_decl:
    case Intrinsic::invariant_start:
    case Intrinsic::invariant_end:
      return true;
    default:
      break;
    }
  }
  return false;
}

unsigned lineOf(const Instruction *I) {
  if (const DebugLoc &DL = I->getDebugLoc())
    return DL.getLine();
  return 0;
}

struct LoopFacts {
  std::string File, Function;
  unsigned Line = 0, Col = 0, Depth = 0, NumSubLoops = 0;
  bool Innermost = true, MultiExit = false, TripCountComputable = true;
  unsigned NumLoads = 0, NumStores = 0, NumCalls = 0;
  unsigned NumInductions = 0, NumBenignPhis = 0;
  SmallVector<std::string, 4> Reductions;
  SmallVector<std::string, 8> SerialReasons;
  SmallVector<std::string, 8> UnknownReasons;
  json::Array ProvenDeps, MayDeps;
  unsigned DepPairsTested = 0;
  double AnalysisMicros = 0;
};

class HFLoopAnalysisPass : public PassInfoMixin<HFLoopAnalysisPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    if (F.isDeclaration())
      return PreservedAnalyses::all();
    auto &LI = FAM.getResult<LoopAnalysis>(F);
    if (LI.empty())
      return PreservedAnalyses::all();
    auto &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
    auto &DI = FAM.getResult<DependenceAnalysis>(F);
    auto &AA = FAM.getResult<AAManager>(F);
    auto &AC = FAM.getResult<AssumptionAnalysis>(F);
    auto &DT = FAM.getResult<DominatorTreeAnalysis>(F);

    const char *Env = std::getenv("HF_STATIC_OUT");
    std::string OutPath = Env ? Env : "hf_static.jsonl";
    std::error_code EC;
    raw_fd_ostream OS(OutPath, EC, sys::fs::OF_Append | sys::fs::OF_Text);
    if (EC) {
      errs() << "hf-loop-analysis: cannot open " << OutPath << ": "
             << EC.message() << "\n";
      return PreservedAnalyses::all();
    }

    for (Loop *L : LI.getLoopsInPreorder()) {
      DebugLoc Start = L->getStartLoc();
      if (Start && Start.getInlinedAt())
        continue; // inlined copy of a loop reported elsewhere
      auto T0 = std::chrono::steady_clock::now();
      LoopFacts LF = analyzeLoop(*L, F, SE, DI, AA, AC, DT);
      auto T1 = std::chrono::steady_clock::now();
      LF.AnalysisMicros =
          std::chrono::duration<double, std::micro>(T1 - T0).count();
      if (Start) {
        LF.Line = Start.getLine();
        LF.Col = Start.getCol();
        if (auto *Scope = dyn_cast_or_null<DIScope>(Start.getScope()))
          LF.File = Scope->getFilename().str();
      }
      emit(OS, LF);
    }
    return PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }

private:
  static Verdict decide(const LoopFacts &LF) {
    if (!LF.SerialReasons.empty())
      return Verdict::Serial;
    if (!LF.UnknownReasons.empty())
      return Verdict::Unknown;
    return Verdict::Parallel;
  }

  static void addOnce(SmallVectorImpl<std::string> &V, const std::string &S) {
    for (auto &E : V)
      if (E == S)
        return;
    V.push_back(S);
  }

  // Is the memory read by load LdI written anywhere inside L?
  static bool isWrittenInLoop(LoadInst *LdI, Loop &L, AAResults &AA) {
    MemoryLocation Loc = MemoryLocation::get(LdI);
    for (BasicBlock *BB : L.blocks())
      for (Instruction &I : *BB)
        if (I.mayWriteToMemory() && isModSet(AA.getModRefInfo(&I, Loc)))
          return true;
    return false;
  }

  // Does V (transitively, through instructions inside L) depend on the header
  // PHI P?  Other header PHIs (inductions, other recurrences) end the walk.
  static bool dependsOnPhi(Value *V, PHINode *P, Loop &L,
                           SmallPtrSetImpl<Value *> &Seen) {
    if (V == P)
      return true;
    auto *I = dyn_cast<Instruction>(V);
    if (!I || !L.contains(I) || !Seen.insert(I).second)
      return false;
    if (auto *Q = dyn_cast<PHINode>(I))
      if (Q->getParent() == L.getHeader())
        return false;
    for (Value *Op : I->operands())
      if (dependsOnPhi(Op, P, L, Seen))
        return true;
    return false;
  }

  LoopFacts analyzeLoop(Loop &L, Function &F, ScalarEvolution &SE,
                        DependenceInfo &DI, AAResults &AA, AssumptionCache &AC,
                        DominatorTree &DT) {
    LoopFacts LF;
    LF.Function = F.getName().str();
    LF.Depth = L.getLoopDepth();
    LF.NumSubLoops = L.getSubLoops().size();
    LF.Innermost = L.isInnermost();

    // ---- 1. Control structure: OpenMP needs a canonical, single-exit loop.
    SmallVector<BasicBlock *, 4> Exiting;
    L.getExitingBlocks(Exiting);
    if (Exiting.size() > 1) {
      LF.MultiExit = true;
      addOnce(LF.SerialReasons,
              "early exit (break/return): loop is not in OpenMP canonical form");
    }
    const SCEV *BTC = SE.getBackedgeTakenCount(&L);
    if (isa<SCEVCouldNotCompute>(BTC)) {
      LF.TripCountComputable = false;
      if (!LF.MultiExit)
        addOnce(LF.UnknownReasons, "trip count is not computable by SCEV");
    }

    // ---- 2. Scalar recurrences carried through header PHIs.
    // IVDescriptors require loop-simplify + LCSSA form (the driver runs
    // `loop-simplify,lcssa` first); be defensive anyway.
    BasicBlock *Latch = L.getLoopLatch();
    bool Simplified = L.isLoopSimplifyForm() && L.isLCSSAForm(DT);
    if (!Simplified)
      addOnce(LF.UnknownReasons, "loop not in simplified/LCSSA form");
    for (PHINode &P : L.getHeader()->phis()) {
      InductionDescriptor ID;
      if (InductionDescriptor::isInductionPHI(&P, &L, &SE, ID)) {
        ++LF.NumInductions;
        continue;
      }
      RecurrenceDescriptor RD;
      if (!Simplified)
        continue;
      if (RecurrenceDescriptor::isReductionPHI(&P, &L, RD, nullptr, &AC, &DT,
                                               &SE)) {
        LF.Reductions.push_back(recurKindName(RD.getRecurrenceKind()));
        continue;
      }
      if (!Latch) {
        addOnce(LF.UnknownReasons, "loop has no unique latch");
        continue;
      }
      Value *FromLatch = P.getIncomingValueForBlock(Latch);
      // Value forwarded from a load of read-only memory (an artefact of
      // GVN/LICM, not a real dependence) -- unless the load address itself
      // depends on the carried value (pointer chasing, p = next[p]), which is
      // a genuine recurrence.  [v2.1 fix: found by HF-LoopBench-X]
      if (auto *Ld = dyn_cast<LoadInst>(FromLatch)) {
        SmallPtrSet<Value *, 16> Seen;
        if (L.contains(Ld) && !isWrittenInLoop(Ld, L, AA) &&
            !dependsOnPhi(Ld->getPointerOperand(), &P, L, Seen)) {
          ++LF.NumBenignPhis;
          continue;
        }
      }
      // PHI only consumed after the loop (last-value / lastprivate).
      bool UsedInLoop = false;
      for (User *U : P.users())
        if (auto *UI = dyn_cast<Instruction>(U))
          if (L.contains(UI))
            UsedInLoop = true;
      if (!UsedInLoop) {
        ++LF.NumBenignPhis;
        continue;
      }
      // Conditional update of the carried value (select / merge PHI that
      // takes the old value on one path): a min/max/last-value pattern that
      // RecurrenceDescriptor did not recognise (e.g. FP max without
      // fast-math).  Not provably serial.
      bool Conditional = false;
      if (auto *Sel = dyn_cast<SelectInst>(FromLatch))
        Conditional = Sel->getTrueValue() == &P || Sel->getFalseValue() == &P;
      else if (auto *MP = dyn_cast<PHINode>(FromLatch))
        for (Value *In : MP->incoming_values())
          if (In == &P)
            Conditional = true;
      unsigned UpdLine = 0;
      if (auto *UI = dyn_cast<Instruction>(FromLatch))
        UpdLine = lineOf(UI);
      if (Conditional) {
        addOnce(LF.UnknownReasons,
                "conditionally updated scalar (possible min/max/last-value "
                "reduction) at line " + std::to_string(UpdLine));
        continue;
      }
      addOnce(LF.SerialReasons,
              "scalar loop-carried recurrence (neither induction nor "
              "reduction) updated at line " + std::to_string(UpdLine));
    }

    // ---- 3. Memory operations and calls.
    SmallVector<Instruction *, 32> MemOps;
    bool HasWrites = false;
    for (BasicBlock *BB : L.blocks()) {
      for (Instruction &I : *BB) {
        if (!I.mayReadOrWriteMemory() || isIgnorableIntrinsic(I))
          continue;
        if (auto *Ld = dyn_cast<LoadInst>(&I)) {
          ++LF.NumLoads;
          if (Ld->isVolatile() || Ld->isAtomic())
            addOnce(LF.UnknownReasons, "volatile/atomic load");
          MemOps.push_back(&I);
        } else if (auto *St = dyn_cast<StoreInst>(&I)) {
          ++LF.NumStores;
          HasWrites = true;
          if (St->isVolatile() || St->isAtomic())
            addOnce(LF.UnknownReasons, "volatile/atomic store");
          MemOps.push_back(&I);
        } else if (auto *CB = dyn_cast<CallBase>(&I)) {
          ++LF.NumCalls;
          Function *Callee = CB->getCalledFunction();
          StringRef Name = Callee ? Callee->getName() : StringRef("<indirect>");
          if (CB->doesNotAccessMemory())
            continue;
          if (orderDependentCalls().contains(Name)) {
            addOnce(LF.SerialReasons,
                    ("order-dependent library call '" + Name + "'").str());
            continue;
          }
          if (threadSafeCalls().contains(Name))
            continue;
          if (CB->onlyReadsMemory()) {
            MemOps.push_back(&I); // checked against writes below
            continue;
          }
          HasWrites = true;
          if (isa<MemIntrinsic>(CB))
            addOnce(LF.UnknownReasons,
                    ("memory intrinsic '" + Name + "' not analysable by DA").str());
          else
            addOnce(LF.UnknownReasons,
                    ("call to '" + Name + "' with unknown side effects").str());
        } else {
          addOnce(LF.UnknownReasons, std::string("unanalysed memory instruction '") +
                                         I.getOpcodeName() + "'");
        }
      }
    }

    // ---- 4. Pairwise dependence testing at this loop's level.
    unsigned Level = L.getLoopDepth();
    for (size_t A = 0; A < MemOps.size(); ++A) {
      for (size_t B = A; B < MemOps.size(); ++B) {
        Instruction *Src = MemOps[A], *Dst = MemOps[B];
        bool SrcW = Src->mayWriteToMemory(), DstW = Dst->mayWriteToMemory();
        if (!SrcW && !DstW)
          continue;
        // Read-only call vs. write: use alias analysis directly.
        if (isa<CallBase>(Src) || isa<CallBase>(Dst)) {
          CallBase *CB = dyn_cast<CallBase>(Src) ? cast<CallBase>(Src)
                                                 : cast<CallBase>(Dst);
          Instruction *Other = CB == Src ? Dst : Src;
          if (isa<CallBase>(Other))
            continue; // both calls: covered by the opaque-call reason above
          if (isModOrRefSet(AA.getModRefInfo(CB, MemoryLocation::get(Other)))) {
            addOnce(LF.UnknownReasons,
                    "read-only call may read memory written in the loop");
            json::Object O;
            O["reason"] = "call/memory conflict";
            O["src_line"] = (int64_t)lineOf(Src);
            O["dst_line"] = (int64_t)lineOf(Dst);
            LF.MayDeps.push_back(std::move(O));
          }
          continue;
        }
        ++LF.DepPairsTested;
#ifdef HF_DA_POSSIBLY_LOOP_INDEPENDENT
        std::unique_ptr<Dependence> D = DI.depends(Src, Dst, true);
#else
        std::unique_ptr<Dependence> D = DI.depends(Src, Dst);
#endif
        if (!D)
          continue; // proven independent
        if (D->isInput())
          continue;
        const char *Kind = D->isFlow() ? "flow" : D->isAnti() ? "anti"
                                                              : "output";
        if (D->isConfused()) {
          addOnce(LF.UnknownReasons,
                  "dependence test inconclusive (may-alias or non-affine "
                  "subscript)");
          json::Object O;
          O["reason"] = "confused";
          O["kind"] = Kind;
          O["src_line"] = (int64_t)lineOf(Src);
          O["dst_line"] = (int64_t)lineOf(Dst);
          LF.MayDeps.push_back(std::move(O));
          continue;
        }
        unsigned Levels = D->getLevels();
        if (Level > Levels)
          continue;
        bool OuterAllowsEq = true;
        for (unsigned K = 1; K < Level; ++K)
          if (!(D->getDirection(K) & Dependence::DVEntry::EQ)) {
            OuterAllowsEq = false;
            break;
          }
        if (!OuterAllowsEq)
          continue; // carried by an enclosing loop only
        unsigned Dir = D->getDirection(Level);
        if (!(Dir & (Dependence::DVEntry::LT | Dependence::DVEntry::GT)))
          continue; // loop-independent at this level
        const SCEV *Dist = D->getDistance(Level);
        bool KnownDist = Dist && isa<SCEVConstant>(Dist) &&
                         !cast<SCEVConstant>(Dist)->getValue()->isZero();
        bool Proven = KnownDist ||
                      (D->isConsistent() && Dir != Dependence::DVEntry::ALL &&
                       !D->isScalar(Level));
        json::Object O;
        O["kind"] = Kind;
        O["src_line"] = (int64_t)lineOf(Src);
        O["dst_line"] = (int64_t)lineOf(Dst);
        std::string DirS;
        if (Dir & Dependence::DVEntry::LT) DirS += "<";
        if (Dir & Dependence::DVEntry::EQ) DirS += "=";
        if (Dir & Dependence::DVEntry::GT) DirS += ">";
        O["direction"] = DirS;
        if (KnownDist)
          O["distance"] =
              (int64_t)cast<SCEVConstant>(Dist)->getAPInt().getSExtValue();
        if (Proven) {
          LF.ProvenDeps.push_back(std::move(O));
          addOnce(LF.SerialReasons,
                  std::string("proven loop-carried ") + Kind + " dependence");
        } else {
          O["reason"] = "possible carried dependence";
          LF.MayDeps.push_back(std::move(O));
          addOnce(LF.UnknownReasons,
                  std::string("possible loop-carried ") + Kind +
                      " dependence (direction not resolved)");
        }
      }
    }
    (void)HasWrites;
    return LF;
  }

  void emit(raw_ostream &OS, const LoopFacts &LF) {
    json::Object O;
    O["file"] = LF.File;
    O["function"] = LF.Function;
    O["line"] = (int64_t)LF.Line;
    O["col"] = (int64_t)LF.Col;
    O["depth"] = (int64_t)LF.Depth;
    O["innermost"] = LF.Innermost;
    O["subloops"] = (int64_t)LF.NumSubLoops;
    O["multi_exit"] = LF.MultiExit;
    O["trip_count_computable"] = LF.TripCountComputable;
    O["loads"] = (int64_t)LF.NumLoads;
    O["stores"] = (int64_t)LF.NumStores;
    O["calls"] = (int64_t)LF.NumCalls;
    O["inductions"] = (int64_t)LF.NumInductions;
    O["benign_phis"] = (int64_t)LF.NumBenignPhis;
    json::Array Red;
    for (auto &R : LF.Reductions)
      Red.push_back(R);
    O["reductions"] = std::move(Red);
    json::Array SR, UR;
    for (auto &R : LF.SerialReasons)
      SR.push_back(R);
    for (auto &R : LF.UnknownReasons)
      UR.push_back(R);
    O["serial_reasons"] = std::move(SR);
    O["unknown_reasons"] = std::move(UR);
    O["proven_deps"] = json::Array(LF.ProvenDeps);
    O["may_deps"] = json::Array(LF.MayDeps);
    O["dep_pairs_tested"] = (int64_t)LF.DepPairsTested;
    O["analysis_us"] = LF.AnalysisMicros;
    Verdict V = decide(LF);
    O["verdict"] = verdictName(V);
    std::string Pragma;
    if (V == Verdict::Parallel) {
      Pragma = "#pragma omp parallel for";
      for (auto &R : LF.Reductions)
        Pragma += " reduction(" + R + ":<var>)";
    }
    O["suggested_pragma"] = Pragma;
    OS << json::Value(std::move(O)) << "\n";
  }
};

// ---------------------------------------------------------------------------
// hf-polly-report: after Polly code generation with -polly-parallel, report
// every call that launches a Polly-outlined parallel sub-function, whether the
// launch sits inside a loop of the caller (then the parallelised loop is not
// the outermost loop of the nest) and the source-line range covered by the
// outlined body.  Used only to score the Polly baseline.
class HFPollyReportPass : public PassInfoMixin<HFPollyReportPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    if (F.isDeclaration() || F.getName().contains("polly"))
      return PreservedAnalyses::all();
    auto &LI = FAM.getResult<LoopAnalysis>(F);
    const char *Env = std::getenv("HF_POLLY_OUT");
    std::string OutPath = Env ? Env : "hf_polly.jsonl";
    std::error_code EC;
    raw_fd_ostream OS(OutPath, EC, sys::fs::OF_Append | sys::fs::OF_Text);
    if (EC)
      return PreservedAnalyses::all();
    for (BasicBlock &BB : F)
      for (Instruction &I : BB) {
        auto *CB = dyn_cast<CallBase>(&I);
        if (!CB)
          continue;
        Function *Sub = nullptr;
        for (Value *Arg : CB->args())
          if (auto *AF = dyn_cast<Function>(Arg->stripPointerCasts()))
            if (AF->getName().contains("polly_subfn"))
              Sub = AF;
        if (Function *Callee = CB->getCalledFunction())
          if (Callee->getName().contains("polly_subfn"))
            Sub = Callee;
        if (!Sub)
          continue;
        unsigned Lo = ~0u, Hi = 0;
        for (BasicBlock &SB : *Sub)
          for (Instruction &SI : SB)
            if (const DebugLoc &DL = SI.getDebugLoc())
              if (DL.getLine()) {
                Lo = std::min(Lo, DL.getLine());
                Hi = std::max(Hi, DL.getLine());
              }
        json::Object O;
        O["function"] = F.getName().str();
        O["subfn"] = Sub->getName().str();
        O["in_loop"] = LI.getLoopFor(&BB) != nullptr;
        O["line_lo"] = (int64_t)(Lo == ~0u ? 0 : Lo);
        O["line_hi"] = (int64_t)Hi;
        OS << json::Value(std::move(O)) << "\n";
      }
    return PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};

// ---------------------------------------------------------------------------
// hf-callsite-alias: for every direct call to a function defined in the
// module, report pairs of pointer arguments that are derived from the same
// underlying object (or that alias analysis cannot separate) at that call
// site.  HF-Static itself is intra-procedural; this context fact lets the
// hybrid decision veto loops whose pointer parameters may overlap at a call.
class HFCallSiteAliasPass : public PassInfoMixin<HFCallSiteAliasPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
    if (F.isDeclaration())
      return PreservedAnalyses::all();
    auto &AA = FAM.getResult<AAManager>(F);
    const char *Env = std::getenv("HF_CALLSITE_OUT");
    std::string OutPath = Env ? Env : "hf_callsite.jsonl";
    std::error_code EC;
    raw_fd_ostream OS(OutPath, EC, sys::fs::OF_Append | sys::fs::OF_Text);
    if (EC)
      return PreservedAnalyses::all();
    for (BasicBlock &BB : F)
      for (Instruction &I : BB) {
        auto *CB = dyn_cast<CallBase>(&I);
        if (!CB)
          continue;
        Function *Callee = CB->getCalledFunction();
        if (!Callee || Callee->isDeclaration())
          continue;
        SmallVector<unsigned, 8> Ptrs;
        for (unsigned A = 0; A < CB->arg_size(); ++A)
          if (CB->getArgOperand(A)->getType()->isPointerTy())
            Ptrs.push_back(A);
        json::Array Pairs;
        for (size_t X = 0; X < Ptrs.size(); ++X)
          for (size_t Y = X + 1; Y < Ptrs.size(); ++Y) {
            Value *PA = CB->getArgOperand(Ptrs[X]), *PB = CB->getArgOperand(Ptrs[Y]);
            const Value *UA = getUnderlyingObject(PA), *UB = getUnderlyingObject(PB);
            bool SameObj = UA == UB;
            AliasResult AR = AA.alias(MemoryLocation::getBeforeOrAfter(PA),
                                      MemoryLocation::getBeforeOrAfter(PB));
            if (SameObj || AR != AliasResult::NoAlias) {
              json::Object O;
              O["arg_a"] = (int64_t)Ptrs[X];
              O["arg_b"] = (int64_t)Ptrs[Y];
              O["same_object"] = SameObj;
              O["alias"] = AR == AliasResult::MustAlias ? "must"
                           : AR == AliasResult::PartialAlias ? "partial"
                           : AR == AliasResult::MayAlias ? "may" : "no";
              Pairs.push_back(std::move(O));
            }
          }
        json::Object R;
        R["caller"] = F.getName().str();
        R["callee"] = Callee->getName().str();
        R["line"] = (int64_t)lineOf(&I);
        R["pointer_args"] = (int64_t)Ptrs.size();
        R["overlapping_pairs"] = std::move(Pairs);
        OS << json::Value(std::move(R)) << "\n";
      }
    return PreservedAnalyses::all();
  }
  static bool isRequired() { return true; }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HFLoopAnalysis", "2.0",
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hf-loop-analysis") {
                    FPM.addPass(HFLoopAnalysisPass());
                    return true;
                  }
                  if (Name == "hf-callsite-alias") {
                    FPM.addPass(HFCallSiteAliasPass());
                    return true;
                  }
                  if (Name == "hf-polly-report") {
                    FPM.addPass(HFPollyReportPass());
                    return true;
                  }
                  return false;
                });
          }};
}
