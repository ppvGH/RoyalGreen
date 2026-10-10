# Automated testing

[Documentation index](index.md) · [Validation](development-validation.md) · [Manual checks](manual-testing.md)

## Run the suite

```powershell
.\scripts\run_checks.ps1 -Configuration Debug -BuildApplication
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
```

Linux/macOS use CMake/CTest commands in [installation](installation.md). The test build is independent of the graphical application. Catch2 assertions remain enabled in both configurations.

There are **123 C++ cases**, **25 Python validator unit tests** and two repository checks. CTest reports **126 entries**: the 123 C++ cases, asset integrity, preserved history and one Python unittest container. Entry/assertion counts are not coverage percentages.

## Scope and expected results

| Area | Assertions |
| --- | --- |
| Geometry / meshes | Analytic intersections, two-sided hits, tolerance/barycentric boundaries, scaling/nearest hit; owned snapshots survive vertex destruction; zero-area faces excluded; invalid indices/geometry fail |
| Boxes / movement | Strict overlap, edge contact, translation invariance, finite positive extents, elapsed-time displacement |
| Input | Controlled press/hold/release/up, release once, registration/rebinding/shared actions |
| Camera | Projection/view, normalized/clamped basis, grounding captures height; equal one-second travel at 30/60/120 updates |
| Animation | One-based UV/facing, complete intervals/remainder, looping/nonlooping final frames, crossed shoot events, invalid sheet/FPS/time |
| Components | Jump/attack/hit/death, consecutive attacks, terminal cat, immediate reset bounds/flags, projectile cleanup and hits once |
| GameSession | Fixed camera/both margins, pause/resume/help/reset, terminal menu/rematch |
| Startup / options | Injected GLFW/window failure, null GLAD; config/CLI validation/precedence, relative paths, executable-relative assets and missing files |
| CPU ownership | Noncopyable owners, safe zero-handle moves, no ghost resources on failure |
| Repository | Named asset references, PNG/sprite/OBJ/MTL contracts; retained ancestors, protected refs, unchanged original assets/GLAD |
| Validator fixtures | Positive/negative asset/source examples, damaged images and genuine code changes for the legacy checker |

Fixtures replace GLFW polling/window calls; the CPU suite opens no native window and issues no GPU calls. Actors use ID-zero textures injected into the real map. Exact-margin movement tests avoid starting another AI barrage; combat coordination has separate cases.

`check_source_unchanged.py` remains for `sac4ap`, whose code changes are comments only. The development guard is `check_history_preserved.py`: approved production changes are allowed while protected ancestry/refs/assets remain checked.

## Real OpenGL integration

```powershell
.\build\app\Release\royalgreen.exe --graphics-check
```

This needs a driver/desktop. Startup loads actual shaders/textures/models, creates scene/game targets and renders once. Further checks exercise invalid shader/reload preservation, missing model/image handling, texture/mesh moves, allocation/deletion of GL handles and absence of a pending GL error. Normal `--exit-after 2` checks the frame loop and clean shutdown separately.

CI builds/tests Windows Debug/Release and native Linux/macOS. Linux graphics checks use Xvfb/Mesa; hosted Windows/macOS builds do not establish hardware gameplay quality. The Windows package verifier checks PE x64, every asset byte, config/notices and emits ZIP SHA256 plus per-asset manifest. `check_windows_package.ps1 -Archive ZIP -Graphics` extracts to a path with spaces and checks CLI/error/graphics/normal sessions from an unrelated working directory; omit `-Graphics` for a runner without a graphics desktop.

## Evidence and limits

JUnit/CTest logs are under `build/tests/`; CI uploads diagnostics. [Development validation](development-validation.md) identifies completed runs. Interactive checklist fields stay blank until performed.

Tests do not prove every UI transition, pixel correctness, focus/driver behavior, continuous collision, repeated full-scene creation, or absence of all leaks. No whole-project coverage figure is asserted. Optional coverage reports concern linked modules only; Doxygen is supplemental reference.
