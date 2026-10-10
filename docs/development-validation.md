# Development validation

[Documentation index](index.md) · [Tests](testing.md) · [Original branch record](validation.md)

Actual local checks completed on **10 October 2026 (Europe/Rome)** on `sac4ap-dev`. This record does not mark the separate interactive checklist as performed.

## Preservation and environment

- Protected `main`: `20ae12414fb991aa6d0e4b172df25e5239fe81b8` (2025-12-08), all **263** original reachable commits retained.
- Protected `sac4ap`: `b3b13c28737518e8082cac0408ccc4de26a463c5`.
- Development starts from `sac4ap`; ancestry/protected-ref/asset guard passes. No historical dates/content rewritten. Runtime assets and bundled GLAD remain original.
- Local Windows 11 Home x64 10.0.26300, VS 2022/MSVC 19.44.35207.1 v143, SDK 10.0.26100, CMake 3.31.6-msvc6, Python 3.13.13.
- Real context renderer: **NVIDIA GeForce RTX 4060 Laptop GPU/PCIe/SSE2**.

## Completed local checks

| Check | Result / evidence under ignored build/ |
| --- | --- |
| CPU suite Debug | **126/126 CTest entries PASS**, `dev-ctest-Debug.log`, `tests/dev-Debug.xml` |
| CPU suite Release | **126/126 PASS**, `dev-ctest-Release.log`, `tests/dev-Release.xml` |
| Test population | 123 C++ cases; 25 Python validator fixtures inside one CTest container; asset/history checks separately |
| App build Debug/Release | **PASS**, `dev-app-Debug.log`, `dev-package.log` |
| Real graphics Debug | **PASS**, actual assets/model import + game/room draw, ownership/moved-from cube reuse/failed reload/missing files/handle deletion/no pending GL error; `dev-graphics-Debug.stdout.log` |
| Windows Release package | **PASS**, CPack ZIP, AMD64 PE check, all **34** asset files byte-identical, config and **8** upstream notices |
| Relocated extracted package | **PASS**, path with spaces; launched from system temp directory, not checkout/package working directory |
| Packaged CLI/error | `--help` exit0; missing `--assets` exit1 with diagnostic |
| Packaged real graphics / ordinary loop | `--graphics-check` exit0; `--width 960 --height 640 --exit-after 2` exit0 |
| Runtime DLL inspection | Only KERNEL32/USER32/GDI32/SHELL32/IMM32 in dumpbin `/DEPENDENTS`; no separate MSVC runtime/library DLL needed |
| Original references/assets | **PASS**, `check_history_preserved.py`, `verify_assets.py` (26 named paths, six OBJ/MTL libraries) |

The local package is `build/packages/RoyalGreen-0.2.0-Windows-AMD64.zip` (~2.64 MiB), with `.zip.sha256` and `.manifest.json`. SHA256 is recorded alongside each generated archive; rebuilding can change the hash. Logs for extracted-package checks remain under `build/package check <id>/`.

## Remote checks

The [development workflow](../.github/workflows/sac4ap-dev.yml) builds/tests Windows Debug/Release, Ubuntu 24.04 and macOS 15 native ARM/Intel. Windows Release creates an artifact; Linux adds Xvfb/Mesa graphics/normal-session checks. Completed run links/results are recorded after execution; configuration alone is not a success claim.

## Limits

Windows 10 x64 is a target, not a separately observed local machine. No Windows ARM emulation or other Windows GPU validation is claimed. Native macOS build/tests do not establish hardware playability/pixel accuracy. Linux software GL checks do not establish all physical-driver combinations. UI click-through, focus/scaling and repeated full session transitions remain in the unfilled manual checklist. Doxygen/coverage were not measured; no coverage percentage or whole-program leak claim is made. Project-license and asset attribution decisions remain with the owner. No public GitHub release/tag has been created.
