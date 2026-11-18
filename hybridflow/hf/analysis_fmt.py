"""Small formatting helpers shared by the report generator."""


def pct(x, d=1):
    return f"{100 * x:.{d}f}"


def num(n):
    return f"{n:,}"


def esc(s):
    return str(s).replace("_", "\\_").replace("%", "\\%").replace("&", "\\&").replace("#", "\\#")
