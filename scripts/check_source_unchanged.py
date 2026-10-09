#!/usr/bin/env python3
"""Verify that the exam branch changes production sources only through comments.

The reference is the original tip of main; no dates or historical commits are rewritten.
This is a lexical guard, not a proof that arbitrary C++ programs are equivalent.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "20ae12414fb991aa6d0e4b172df25e5239fe81b8"
LEXER = re.compile(
    r'(?P<raw>(?:u8|u|U|L)?R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})\(.*?\)(?P=delimiter)")'
    r'|(?P<literal>(?:u8|u|U|L)?(?:"(?:\\[\s\S]|[^"\\])*"|\x27(?:\\[\s\S]|[^\x27\\])*\x27))'
    r'|(?P<comment>//[^\n]*|/\*[\s\S]*?\*/)'
    r'|(?P<space>\s+)'
    r'|(?P<number>(?:0[xX][0-9a-fA-F\x27]+(?:\.[0-9a-fA-F\x27]*)?(?:[pP][+-]?[0-9\x27]+)?|(?:[0-9][0-9\x27]*(?:\.[0-9\x27]*)?|\.[0-9][0-9\x27]*)(?:[eE][+-]?[0-9\x27]+)?)[a-zA-Z0-9_]*)'
    r'|(?P<identifier>[a-zA-Z_$][a-zA-Z0-9_$]*)'
    r'|(?P<operator>::|->\*|->|\.\*|\.\.\.|<<=|>>=|<<|>>|<=|>=|==|!=|&&|\|\||\+\+|--|\+=|-=|\*=|/=|%=|&=|\|=|\^=|##)'
    r'|(?P<other>.)', re.DOTALL
)


def strip_comments(text: str) -> str:
    """Replace C/C++/GLSL comments with one space, retaining literal contents.

    Block-comment internal newlines do not terminate a preprocessing directive.
    """
    return "".join(" " if item.lastgroup == "comment" else item.group()
                   for item in LEXER.finditer(text))


def cpp_tokens(source: bytes) -> list[str]:
    """Compare tokens, literal contents and conservative whitespace boundaries.

    Raw-string bodies retain backslash/newline contents. Macro stringification
    can observe whitespace between tokens, so changing a separator boundary is
    conservatively rejected even where ordinary C++ would ignore that layout.
    Latin-1 is a reversible byte mapping for the legacy source encodings.
    """
    text = source.decode("latin-1").replace("\r\n", "\n")
    parts, cursor = [], 0
    for item in LEXER.finditer(text):
        if item.lastgroup == "raw":
            parts.append(re.sub(r"\\\n", "", text[cursor:item.start()]))
            parts.append(item.group())
            cursor = item.end()
    parts.append(re.sub(r"\\\n", "", text[cursor:]))
    cleaned = strip_comments("".join(parts))
    tokens, gap = [], False
    for item in LEXER.finditer(cleaned):
        if item.lastgroup == "space":
            gap = True
        elif item.lastgroup != "comment":
            if tokens:
                tokens.append("<separator>" if gap else "<adjacent>")
            tokens.append(item.group())
            gap = False
    directives = []
    for line in cleaned.splitlines():
        if line.lstrip().startswith("#"):
            macro = re.match(r"\s*#\s*define\s+\w+(\()?", line)
            kind = "function_macro" if macro and macro.group(1) else "directive"
            directives.append((kind, tuple(m.group() for m in LEXER.finditer(line)
                                          if m.lastgroup != "space")))
    return tokens + [repr(directives)]


def git(root: Path, *args: str) -> bytes:
    """Use argument lists so branch names and paths never become shell commands."""
    result = subprocess.run(["git", "-C", str(root), *args], capture_output=True, check=True)
    return result.stdout


def check_sources(root: Path, base: str) -> tuple[int, int]:
    """Compare original sources lexically and assets/build files byte for byte."""
    root = root.resolve()
    revision = git(root, "rev-parse", "--verify", f"{base}^{{commit}}").decode().strip()
    paths = git(root, "ls-tree", "-rz", "--name-only", revision).decode().split("\0")
    originals = {path for path in paths if path.startswith("src/")}
    current = {path.relative_to(root).as_posix() for path in (root / "src").rglob("*") if path.is_file()}
    if current != originals:
        raise ValueError(f"production file set changed: added={sorted(current-originals)}, removed={sorted(originals-current)}")
    lexical, exact = 0, 0
    for path in paths:
        if not path:
            continue
        if path.startswith("src/"):
            before = git(root, "show", f"{revision}:{path}")
            if cpp_tokens(before) != cpp_tokens((root / path).read_bytes()):
                raise ValueError(f"non-comment production change: {path}")
            lexical += 1
        elif path.startswith(("res/", "external/")) or path in ("CMakeLists.txt", "CMakeSettings.json"):
            before = git(root, "show", f"{revision}:{path}")
            # Git checkout may convert CRLF; compare tracked text as normalized bytes.
            after = (root / path).read_bytes()
            if Path(path).suffix in (".txt", ".json", ".h", ".c", ".obj", ".mtl", ".vert", ".frag", ".geom"):
                before, after = before.replace(b"\r\n", b"\n"), after.replace(b"\r\n", b"\n")
            if before != after:
                raise ValueError(f"original asset/build file changed: {path}")
            exact += 1
    return lexical, exact


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--base", default=BASELINE, help="original Git commit/ref (default: original main tip)")
    parser.add_argument("--root", type=Path, default=ROOT, help="checkout (default: script's repository)")
    args = parser.parse_args()
    try:
        lexical, exact = check_sources(args.root, args.base)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        return 1
    print(f"PASS: {lexical} production sources have identical tokens; {exact} original asset/build files unchanged")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())