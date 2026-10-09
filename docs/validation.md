# Validation record

[Documentation index](index.md) · [Testing](testing.md) · [Manual checklist](manual-testing.md)

This record separates checks actually run from checks merely configured or
recommended. The local adaptation/validation session took place on **9-10 October
2026**, with the latest results recorded on **10 October 2026** (Europe/Rome).

## Reference and preservation

- Original `main` tip: `20ae12414fb991aa6d0e4b172df25e5239fe81b8`, dated 2025-12-08.
- Original history: 263 reachable commits; `sac4ap` was created from that tip.
- Production guard: **PASS** for 67 original `src/` files with identical C++ tokens,
  and 39 original asset/GLAD/build files with unchanged contents after text line-ending
  normalization. Only useful comments were edited in existing production headers.
- Original root `CMakeLists.txt` and `CMakeSettings.json` are preserved.
- No old commit was rebased, amended, squashed or backdated.

The guard is a lexical comparison with explicit handling of literals, comments,
operators and preprocessing boundaries. It is not a general C++ equivalence
prover. Its own negative fixtures include changed numeric results, string/raw
literals, operators, macro spacing/newlines, deleted sources and changed assets.

## Local environment

| Tool/platform | Observed value |
| --- | --- |
| OS | Windows 11 Home x64, version 10.0.26300 |
| Compiler | Visual Studio 2022, MSVC 19.44.35207.1 (v143) |
| Windows SDK | 10.0.26100.0 |
| CMake / CTest | 3.31.6-msvc6 |
| Python | 3.13.13 |
| OpenGL renderer diagnostic | NVIDIA GeForce RTX 4060 Laptop GPU/PCIe/SSE2 |

Local absolute paths are intentionally omitted: scripts and documented commands
resolve the checkout from their own location or the current project directory.

## Dependency observations

The unchanged application configure fetched:

| Source | Observed Git revision |
| --- | --- |
| GLFW 3.4 | `7b6aead9fb88b3623e3b3725ebb42670cbe4c579` |
| ImGui v1.91.9 | `97428e8ac99e339ce05eee531cf55b77b29ea709` |
| GLM 0.9.9.8 | `bf71a834948186f4097caa076cd2663c69a10e1e` |
| stb master | `2c980bb59875b0d32144a71867fbdebb2f77cd20` |
| Catch2 (test target) | `fa43b77429ba76c462b1898d6cd2f2d7a9416b14` (v3.7.1) |
| Assimp | Downloaded tag archive v6.0.2 selected by the unchanged root project |

Recording the observed stb commit does not pin the application's moving `master`
selection. New test-project dependencies and their selections are defined in
[tests/CMakeLists.txt](../tests/CMakeLists.txt).

## Application checks completed locally

| Command/check | Result |
| --- | --- |
| `cmake -S . -B build/app -G "Visual Studio 17 2022" -A x64` | PASS: configured/generated with original build definitions. |
| `cmake --build build/app --config Debug --parallel 4` | PASS: `royalgreen.exe` compiled/linked. |
| `cmake --build build/app --config Release --parallel 4` | PASS: `royalgreen.exe` compiled/linked. |
| `scripts/smoke_app.ps1 -Configuration Debug` | PASS: renderer reported, no matched startup errors, normal window close, exit 0. |
| `scripts/smoke_app.ps1 -Configuration Release` | PASS: renderer reported, no matched startup errors, normal window close, exit 0. |
| `python scripts/verify_assets.py` | PASS: 26 paths, six OBJ/MTL relationships, PNG chunk integrity and sprite grids. |
| `python scripts/check_source_unchanged.py` | PASS: 67 sources and 39 original asset/build files preserved. |
| Python validator tests | PASS: 25 tests, including deliberately broken temporary inputs. |

Build/smoke logs and generated test reports stay under ignored `build/`; they are
not committed binaries or claim of a new screenshot session. Startup validation
checks the actual executable and driver but does not assert rendered pixels or
interactive gameplay.

## Automatic test and CI record

| Local automatic check | Result |
| --- | --- |
| Debug CTest suite | **PASS: 99/99 entries**, no failures. |
| Release CTest suite | **PASS: 99/99 entries**, no failures. |
| C++ cases within each configuration | 96 Catch2 cases exercising the original implementation. |
| Python validator unit tests | 25 cases, run within the `repository.validators` CTest entry. |
| Asset and production guards | Two additional CTest entries, both passed. |

The combined commands `scripts/run_checks.ps1 -Configuration Debug -BuildApplication`
and `scripts/run_checks.ps1 -Configuration Release -BuildApplication` also compile
and link the original application. JUnit reports are saved under
`build/tests/test-results-Debug.xml` and `test-results-Release.xml`.

### Completed clean-runner validation

The first published implementation commit
`cb8cfb550667cb4b81ee97f893db3711f1726fa3` passed the
[GitHub Actions run](https://github.com/ppvGH/RoyalGreen/actions/runs/37998954041)
on **10 October 2026** (Europe/Rome; 9 October in the UTC runner logs).

| Independent Windows 2022 job | Recorded result |
| --- | --- |
| [Debug](https://github.com/ppvGH/RoyalGreen/actions/runs/37998954041/job/114052025767) | **PASS: 99/99 CTest entries**, original application built, diagnostics uploaded. |
| [Release](https://github.com/ppvGH/RoyalGreen/actions/runs/37998954041/job/114052025960) | **PASS: 99/99 CTest entries**, original application built, diagnostics uploaded. |

Both jobs started from clean clones with full history and fetched dependencies.
The runner used MSVC **19.44.35229.0**, CMake **3.31.6** and Python **3.12.10**.
JUnit reports and CTest logs are downloadable from that run's artifacts. This
establishes independent compilation and scoped automated checks; it does not
establish a second physical-machine play session or GPU rendering correctness.

For subsequent commits, inspect the
[branch workflow results](https://github.com/ppvGH/RoyalGreen/actions/workflows/sac4ap.yml?query=branch%3Asac4ap)
for the exact submitted SHA. This dated record identifies the tested commit
rather than assuming that all future branch states also pass.

## Explicitly not established by this local record

- A full interactive completion of every scenario in the manual checklist.
- A build/play session on a second physical machine.
- Pixel-level correctness across graphics drivers or changed window sizes.
- A whole-application line/branch coverage percentage. Optional OpenCppCoverage
  is not installed here and no percentage was measured.
- Generated Doxygen HTML. Doxygen is not installed in the local environment;
  maintained Markdown/API documentation is available independently.
- Linux/macOS application compatibility or a standalone relocatable binary package.

Append later observations with the exact commit, environment, commands and
outcomes. Keep failed/pending outcomes visible; do not turn expected manual
results into claims of completed tests.