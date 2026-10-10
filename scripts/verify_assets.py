#!/usr/bin/env python3
"""Check packaged asset references without starting an OpenGL context.

This validates files and cross-file contracts, not their rendered appearance.
Run from any working directory: paths default to the repository containing this script.
"""
from __future__ import annotations

import argparse
from pathlib import Path
import re
import struct
import sys
import zlib

from check_source_unchanged import strip_comments

ROOT = Path(__file__).resolve().parents[1]
ASSET_DIRS = {"SHADERS_DIR": "shaders", "MESHES_DIR": "meshes", "TEXTURE_DIR": "textures"}
PATH_PATTERN = re.compile(
    r'inline\s+(?:const\s+)?std::string\s+(\w+)\s*=\s*std::string\((\w+)\)\s*\+\s*"([^"]+)"\s*;'
)


def require_file(path: Path, root: Path) -> None:
    """Reject references that escape the checkout, are absent or are empty."""
    if not path.resolve().is_relative_to(root.resolve()):
        raise ValueError(f"asset reference escapes repository: {path}")
    if not path.is_file() or path.stat().st_size == 0:
        raise ValueError(f"missing or empty asset: {path}")


def png_dimensions(path: Path) -> tuple[int, int]:
    """Read PNG dimensions and verify chunk integrity to catch damaged downloads."""
    data = path.read_bytes()
    if not data.startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError(f"invalid PNG signature: {path}")
    offset, dimensions, has_data, finished = 8, None, False, False
    while offset < len(data):
        if offset + 12 > len(data):
            raise ValueError(f"truncated PNG chunk: {path}")
        length = struct.unpack_from(">I", data, offset)[0]
        kind = data[offset + 4:offset + 8]
        end = offset + 8 + length
        if end + 4 > len(data):
            raise ValueError(f"truncated PNG payload: {path}")
        payload = data[offset + 8:end]
        crc = struct.unpack_from(">I", data, end)[0]
        if zlib.crc32(kind + payload) & 0xFFFFFFFF != crc:
            raise ValueError(f"PNG checksum mismatch: {path}")
        if offset == 8 and kind != b"IHDR":
            raise ValueError(f"PNG does not start with IHDR: {path}")
        if kind == b"IHDR":
            if dimensions is not None or length != 13:
                raise ValueError(f"invalid PNG header: {path}")
            dimensions = struct.unpack_from(">II", payload)
            if not all(dimensions):
                raise ValueError(f"zero-sized PNG: {path}")
        elif kind == b"IDAT":
            has_data = True
        elif kind == b"IEND":
            if length != 0 or end + 4 != len(data):
                raise ValueError(f"invalid PNG end: {path}")
            finished = True
        offset = end + 4
    if dimensions is None or not has_data or not finished:
        raise ValueError(f"incomplete PNG: {path}")
    return dimensions


def check_assets(root: Path) -> tuple[int, int]:
    """Verify path declarations, OBJ/MTL links, shader versions and sheet grids."""
    root = root.resolve()
    declarations = PATH_PATTERN.findall(strip_comments((root / "src/app/path.h").read_text(encoding="utf-8", errors="surrogateescape")))
    if not declarations:
        raise ValueError("no asset path declarations found")
    checked_models = 0
    for name, macro, filename in declarations:
        if macro not in ASSET_DIRS:
            raise ValueError(f"unknown asset directory macro for {name}: {macro}")
        path = root / "res" / ASSET_DIRS[macro] / filename
        require_file(path, root)
        if macro == "SHADERS_DIR":
            shader = strip_comments(path.read_text(encoding="utf-8", errors="surrogateescape"))
            versions = re.findall(r"^\s*#version\s+(\d+)(?:\s+(\w+))?\s*$", shader, re.MULTILINE)
            if (len(versions) != 1 or versions[0][0] != "330" or versions[0][1] not in ("", "core")
                    or not re.match(r"^\s*#version\b", shader)):
                raise ValueError(f"expected GLSL 330 shader: {path}")
        elif macro == "MESHES_DIR":
            model = path.read_text(encoding="utf-8", errors="surrogateescape")
            if not re.search(r"^vn\s", model, re.MULTILINE):
                raise ValueError(f"model lacks normals required by importer: {path}")
            if not re.search(r"^f\s", model, re.MULTILINE):
                raise ValueError(f"model has no faces: {path}")
            libraries = re.findall(r"^mtllib\s+(.+)$", model, re.MULTILINE)
            if not libraries:
                raise ValueError(f"model has no material library: {path}")
            material_names = set()
            for library in libraries:
                material = path.parent / library.strip()
                require_file(material, root)
                text = material.read_text(encoding="utf-8", errors="surrogateescape")
                material_names.update(re.findall(r"^newmtl\s+(\S+)", text, re.MULTILINE))
            used = set(re.findall(r"^usemtl\s+(\S+)", model, re.MULTILINE))
            if used - material_names:
                raise ValueError(f"undefined OBJ materials in {path}: {sorted(used - material_names)}")
            checked_models += 1
        else:
            png_dimensions(path)
    # Sheet subdivision is a data contract: partial cells would sample adjacent sprites.
    constants = strip_comments((root / "src/app/game2D/game_data.h").read_text(encoding="utf-8", errors="surrogateescape"))
    for character in ("player", "cat"):
        rows = int(re.search(rf"{character}SheetRows\s*=\s*(\d+)", constants).group(1))
        columns = int(re.search(rf"{character}SheetColumns\s*=\s*(\d+)", constants).group(1))
        if rows <= 0 or columns <= 0:
            raise ValueError(f"invalid {character} sheet subdivision")
        width, height = png_dimensions(root / "res/textures" / f"{character}_spritesheet.png")
        if width % columns or height % rows:
            raise ValueError(f"{character} sheet {width}x{height} cannot be split into {rows}x{columns}")
    return len(declarations), checked_models


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT, help="checkout to check (default: script's repository)")
    args = parser.parse_args()
    try:
        count, models = check_assets(args.root)
    except (OSError, ValueError, AttributeError, struct.error) as error:
        print(f"FAIL: {error}", file=sys.stderr)
        return 1
    print(f"PASS: {count} asset paths, {models} OBJ material libraries, PNG checksums and sprite grids")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())