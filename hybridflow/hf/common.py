"""Shared helpers: paths, JSON-lines I/O, environment capture."""
import json
import os
import platform
import subprocess
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def read_jsonl(path):
    out = []
    if not os.path.exists(path):
        return out
    with open(path) as f:
        for line in f:
            line = line.strip()
            if line:
                out.append(json.loads(line))
    return out


def write_jsonl(path, rows):
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "w") as f:
        for r in rows:
            f.write(json.dumps(r) + "\n")


def append_jsonl(path, row):
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "a") as f:
        f.write(json.dumps(row) + "\n")
        f.flush()


def write_json(path, obj):
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, "w") as f:
        json.dump(obj, f, indent=2, sort_keys=False)


def read_json(path, default=None):
    if not os.path.exists(path):
        return default
    with open(path) as f:
        return json.load(f)


def sh(cmd, timeout=None, env=None, cwd=None):
    """Run a command; return (rc, stdout, stderr, seconds)."""
    t0 = time.perf_counter()
    try:
        p = subprocess.run(cmd, shell=isinstance(cmd, str), capture_output=True, text=True,
                           timeout=timeout, env=env, cwd=cwd, errors="replace")
        return p.returncode, p.stdout, p.stderr, time.perf_counter() - t0
    except subprocess.TimeoutExpired as e:
        return -999, (e.stdout or "") if isinstance(e.stdout, str) else "", "TIMEOUT", time.perf_counter() - t0


def loop_key(file, line):
    return f"{os.path.basename(file)}:{int(line)}"


def environment(cc=None, opt=None):
    """Hardware / toolchain description recorded with every result file."""
    env = {"platform": platform.platform(), "machine": platform.machine(),
           "python": platform.python_version(), "time_utc": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}
    if platform.system() == "Darwin":
        for key, cmd in (("cpu", "sysctl -n machdep.cpu.brand_string"), ("ncpu", "sysctl -n hw.ncpu"),
                         ("perf_cores", "sysctl -n hw.perflevel0.physicalcpu"),
                         ("eff_cores", "sysctl -n hw.perflevel1.physicalcpu"),
                         ("mem_bytes", "sysctl -n hw.memsize"), ("macos", "sw_vers -productVersion")):
            env[key] = sh(cmd)[1].strip()
    else:
        env["cpu"] = sh("uname -m")[1].strip()
        env["ncpu"] = str(os.cpu_count())
    if cc:
        env["cc"] = sh([cc, "--version"])[1].split("\n")[0]
    if opt:
        env["opt"] = " ".join(sh([opt, "--version"])[1].split("\n")[0:2]).strip()
    return env
