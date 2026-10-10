#!/usr/bin/env python3
"""Check the actual Windows archive against source assets and emit SHA256 evidence."""
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import struct
import zipfile

ROOT = Path(__file__).resolve().parents[1]
NOTICES = ("GLFW-LICENSE.md", "IMGUI-LICENSE.txt", "ASSIMP-LICENSE.txt",
           "STB-LICENSE.txt", "GLM-LICENSE.txt", "GLAD-notice.h", "KHR-notice.h", "ZLIB-LICENSE.txt")

def verify(archive):
    with zipfile.ZipFile(archive) as package:
        names = package.namelist()
        if len(set(names)) != len(names):
            raise ValueError("Duplicate archive entries")
        for name in names:
            parts = PurePosixPath(name)
            if parts.is_absolute() or ".." in parts.parts or "\\" in name or ":" in name:
                raise ValueError("Invalid archive path: " + name)
        candidates = [name for name in names if name.endswith("/royalgreen.exe") or name == "royalgreen.exe"]
        if len(candidates) != 1:
            raise ValueError("Expected exactly one royalgreen.exe")
        prefix = candidates[0][:-len("royalgreen.exe")]
        executable = package.read(candidates[0])
        if len(executable) < 64 or executable[:2] != b"MZ":
            raise ValueError("Invalid PE executable")
        offset = struct.unpack_from("<I", executable, 60)[0]
        if offset + 6 > len(executable) or executable[offset:offset+4] != b"PE\0\0" or struct.unpack_from("<H", executable, offset+4)[0] != 0x8664:
            raise ValueError("Expected a Windows AMD64 executable")
        required = ["release-readme.md", "asset-provenance.md", "royalgreen.cfg.example"] + ["third-party/" + name for name in NOTICES]
        for relative in required:
            if not package.read(prefix + relative):
                raise ValueError("Empty package file: " + relative)
        hashes = {}
        for asset in sorted((ROOT / "res").rglob("*")):
            if asset.is_file():
                relative = asset.relative_to(ROOT).as_posix()
                expected = asset.read_bytes()
                actual = package.read(prefix + relative)
                if actual != expected:
                    raise ValueError("Packaged asset differs: " + relative)
                hashes[relative] = hashlib.sha256(actual).hexdigest()
        for name in names:
            if name.startswith(prefix + "res/") and not name.endswith("/") and name[len(prefix):] not in hashes:
                raise ValueError("Unexpected packaged asset: " + name)
    return hashes

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path)
    args = parser.parse_args()
    hashes = verify(args.archive)
    digest = hashlib.sha256(args.archive.read_bytes()).hexdigest()
    args.archive.with_suffix(".zip.sha256").write_text(digest + "  " + args.archive.name + "\n", encoding="utf-8")
    args.archive.with_suffix(".manifest.json").write_text(json.dumps({"archive": args.archive.name, "sha256": digest, "assets": hashes}, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: Windows x64 executable, {len(hashes)} unchanged assets, configuration and {len(NOTICES)} notices; SHA256={digest}")

if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, KeyError, zipfile.BadZipFile) as error:
        raise SystemExit("FAIL: " + str(error))
