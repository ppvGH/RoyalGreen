# Contributing and maintenance

[Documentation index](index.md) · [Architecture](architecture.md) · [Testing](testing.md)

## Scope of the assessment branch

`sac4ap` was created from main with the full existing history. The assessment additions document and test the original application without changing its production behavior. Keep application source, original root CMake logic, GLAD, and runtime assets intact unless a future task explicitly authorizes a functional change.

The original reference commit is `20ae12414fb991aa6d0e4b172df25e5239fe81b8`, dated **2025-12-08**. New documentation/tests/checks should use new commits with their actual dates. Do not rewrite old commit dates, amend historical work to imply earlier activity, squash away the original history for the assessment, or describe the gap as an exact year.

## Development setup and checks

Use the [installation guide](installation.md) and a fresh `sac4ap` checkout. Keep application output in `build/app` and regression output in `build/tests` so that the original application project and the separate test project remain independent.

Before submitting changes, run:

```powershell
python scripts/verify_assets.py
python scripts/check_source_unchanged.py
cmake -S tests -B build/tests -G "Visual Studio 17 2022" -A x64
cmake --build build/tests --config Debug --parallel
ctest --test-dir build/tests -C Debug --output-on-failure
```

The [preservation check](../scripts/check_source_unchanged.py) defaults to the immutable original SHA. `python scripts/check_source_unchanged.py --base main` explicitly selects local main as the baseline; remember that a branch name can move. Read the script's reported scope/limitations before treating token comparison as a proof of all possible behavioral equivalence.

Use [run_checks.ps1](../scripts/run_checks.ps1) to run the Python/C++ checks and, optionally, compile the application:

```powershell
.\scripts\run_checks.ps1 -Configuration Debug
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
```

Use [smoke_app.ps1](../scripts/smoke_app.ps1) only on a desktop with OpenGL support after building the app. It checks startup/clean shutdown and diagnostics, not gameplay. [coverage.ps1](../scripts/coverage.ps1) and [Doxyfile](../Doxyfile) are optional; unavailable/unexecuted tooling must remain unverified and cannot justify a claimed coverage percentage.

The Windows CI configuration is [sac4ap.yml](../.github/workflows/sac4ap.yml). A workflow file is not evidence of a successful run; link completed job results in the validation record.

Inspect the diff before committing:

```powershell
git status --short
git diff --check
git diff --stat
```

Select only intended files for staging. Do not commit build outputs, dependency downloads, executables, caches, private local paths, or temporary logs. Leave commit/push actions to the authorized workflow.

## Documentation standards

Write reader-facing documentation in English. Keep [README](../README.md) as a short landing page and [the index](index.md) as the documentation hub. Describe what each feature does, how to use it, its actual defaults, and a working example.

Use the source as the authority for bindings, menu labels, APIs, units, numerical boundaries, and failure behavior. Link headers/implementations and keep examples aligned with the existing signatures. Do not add imaginary executable flags, settings files, Linux/macOS support, or a standalone installer.

When a behavior is incomplete, state the limitation and the available evidence. Keep requirements, expected outcomes, executed checks, and pending checks distinguishable. Existing screenshots illustrate appearance; they do not prove a new validation run. Optional generated API documentation is supplemental to the written guides and does not establish coverage.

## Testing and validation standards

Test meaningful behavior and boundaries: ray/triangle tolerance and scaling, nearest-hit search, strict box overlap, elapsed-time movement, one-based UV animation, input transitions, ownership-based collision, projectile removal, state/reset behavior, and asset/name contracts. Pure graphics draw calls need a context/visual validation, but testable calculations embedded in rendering/import modules still require coverage.

List every uncovered interface/behavior honestly in [testing](testing.md). Do not present a selection of unit tests as complete API coverage. Manual checks complement automated tests for graphical/menu behavior; they cannot replace deterministic tests where those are feasible.

When a check runs, record its date, commit, tools, command, exit/result, and evidence in [validation](validation.md). For an interactive session, fill in [manual-testing](manual-testing.md) or reference a completed record. Keep a second-machine run pending until it is actually performed.

A reported failure should include a minimal reproduction and the expected/observed outcomes. Functional fixes to existing production defects should be proposed separately from preservation-focused assessment work; do not silently alter code to make a test pass.

## Pull-request content

Describe the concrete change and why a reader/reviewer needs it. List commands actually run and their outcomes, then identify remaining gaps. Use actual dates for new history and link existing evidence rather than inventing chronology, test success, licensing, or portability claims.
