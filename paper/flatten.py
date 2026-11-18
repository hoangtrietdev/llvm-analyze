#!/usr/bin/env python3
"""Inline every \\input{...} of main.tex into one self-contained .tex file."""
import os, re, sys
here = os.path.dirname(os.path.abspath(__file__))
def expand(path):
    with open(path) as f:
        text = f.read()
    def rep(m):
        name = m.group(1)
        p = os.path.join(here, name if name.endswith(".tex") else name + ".tex")
        return f"% ---- begin {name} ----\n" + expand(p) + f"\n% ---- end {name} ----"
    return re.sub(r"^\s*\\input\{([^}]+)\}", rep, text, flags=re.M)
out = expand(os.path.join(here, "main.tex"))
with open(os.path.join(here, sys.argv[1] if len(sys.argv) > 1 else "main_flat.tex"), "w") as f:
    f.write(out)
