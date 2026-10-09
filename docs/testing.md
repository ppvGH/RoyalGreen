# Testing

[Documentation index](index.md) · [Installation](installation.md) · [Manual graphics checks](manual-testing.md) · [Recorded validation](validation.md)

The suite compiles the **original implementation files** into a separate C++17
executable. It does not reimplement the algorithms, include a replacement game,
or change the application's original `CMakeLists.txt`.

## Run the complete automatic suite

From a PowerShell terminal in the checkout, with the tools listed in the
[installation guide](installation.md):

```powershell
.\scripts\run_checks.ps1 -Configuration Debug
.\scripts\run_checks.ps1 -Configuration Release
```

Add `-BuildApplication` to compile the interactive application too. The script
stops on the first failed command and saves a JUnit report in
`build/tests/test-results-Debug.xml` or `test-results-Release.xml`.
`-Jobs 2` reduces parallel build/test load on a smaller machine.

Equivalent commands without the wrapper are:

```powershell
cmake -S tests -B build/tests -G "Visual Studio 17 2022" -A x64
cmake --build build/tests --config Debug --parallel 4
ctest --test-dir build/tests -C Debug --output-on-failure
```

Use the matching configuration in both commands. A Release build is tested as
well because plain C/C++ `assert` may disappear with `NDEBUG`; Catch2 assertions
remain active. The first configure downloads the [test dependencies](dependencies.md).
Do not assume that an offline, fresh checkout can fetch them.

List or select focused tests with:

```powershell
ctest --test-dir build/tests -C Debug -N
.\build\tests\Debug\royalgreen_tests.exe "[geometry]"
.\build\tests\Debug\royalgreen_tests.exe "[camera]"
python -m unittest discover -s tests -p test_repository_checks.py -v
```

## What is checked

| Area | Expected behavior and independent checks |
| --- | --- |
| `Vertex` and `TriangleData` | Attribute values/layout, indexed edges and normals, and reference lifetime contracts. |
| `Ray` / `mollerTrumbore` | Analytically located hits, misses outside the triangle, parallel/behind rays, edge/vertex tolerance, reversed winding, non-unit directions, and unchanged output on a miss. |
| `BoundingBox` / `intersectBB` | Corner construction, position updates with preserved size, overlap/containment/separation, strict edge contact, symmetry and translation invariance. |
| `InputManager` | Registered/unregistered inputs, keyboard and mouse polling, Released/Pressed/Held transitions, repeated release and re-registration. |
| `ActionMap` | Binding/rebinding, missing bindings, shared input mappings and forwarding of the three query states. |
| `Camera` | Defaults/custom construction, position/front/aspect/grounded setters, movement, view/projection matrices, yaw/pitch conventions and mouse pitch limits. |
| `Animator` | One-based sheet UV coordinates, horizontal mirroring, frame timing, resetting clips, looping, last-frame completion and the existing large-time-step behavior. |
| `GameObj` / `stateToString` | Position/size/velocity access, signed constant-velocity motion, zero time, composable time steps, and all state labels including the fallback. |
| CPU `Mesh` methods | Analytic centroid/normal, closest ray parameter independent of triangle order, preserved output on misses/empty caches, and explicit shader binding without GPU upload. |
| Gameplay components | Real player/cat movement, jump/attack/hit/reset logic; projectile motion/status/ownership; manager boundaries/removal; and ownership-directed collisions with default ID-zero textures. |
| Asset integration | Actual files referenced in `app/path.h`, GLSL version, OBJ normals/faces and MTL names, PNG chunk checksums and compatible sprite grids. |
| Production preservation | Every original `src/` file has the same C++ tokens; original assets, GLAD and application build files match the baseline. |
| Tool validation | Positive and deliberately damaged temporary fixtures prove that resource and history checks can fail for the right reason. |

Tests use descriptive names, explain their purpose and compare floating-point
results with tolerances. Deterministic property checks use fixed samples rather
than unseeded random input. Multiple component assertions inside a vector/matrix
comparison describe one contract; they are not unrelated checks bundled together.

### Input test boundary

[glfw_input_fake.cpp](../tests/glfw_input_fake.cpp) replaces **only**
`glfwGetKey` and `glfwGetMouseButton`. It uses the real GLFW declarations and
records which polling function was called. The real `InputManager` and `ActionMap`
implementations are linked unmodified. Each fixture resets fake state.

These tests establish polling/state logic. They do not establish that an actual
operating-system event was delivered to a real window. That boundary is exercised
in the [manual checklist](manual-testing.md).

### Current regression contracts

Several surprising legacy behaviors are documented and tested rather than fixed:
`Released` is persistent, `justFinished` is therefore not an edge event, animation
updates advance at most one frame and discard elapsed remainder, and a small
negative ray parameter within `1e-6` may be accepted. Additional characterization
cases cover immediate attack rearming, Dead-cat updates, deferred bounding-box
refresh and repeated collisions before inactive-projectile cleanup. A passing
characterization test means the existing behavior is preserved; it does not mean
these design choices are ideal. See the [improvement report](improvement-report.md).

## Separate graphics smoke check

On a Windows desktop with an OpenGL 3.3 driver, after building the application:

```powershell
.\scripts\smoke_app.ps1 -Configuration Debug
```

This starts the original executable, checks its renderer diagnostic, allows eight
seconds for startup, closes its window normally and checks exit status and error
logs. `-DurationSeconds 15` allows a slower machine more startup time. Logs are in
`build/smoke/`. The smoke check needs a desktop and driver and is deliberately
opt-in. It does **not** assess images, user interaction or gameplay outcomes.

## Coverage and its limits

The automated C++ target links original geometry, input, camera, animation,
mesh and gameplay implementations, plus the real graphics/resource code needed
to link them. Gameplay fixtures put default ID-zero textures in the original
ResourceManager maps. The real `getTexture`, `Sprite`, `Player`, `Cat`,
`Projectile`, `ProjectileManager` and `CollisionSystem` implementations run;
texture loading and draw calls are not invoked. Fixtures clear their maps and
owned projectiles between cases. This checks CPU behavior, not GPU rendering.

The suite does not instantiate the full `Scene`/`Game` coordinators or execute
Assimp import, actual shader/texture loading, GPU allocation/drawing, callbacks
and menu transitions. Some of these contain logic as well as visualization.
They remain an explicit coverage gap, not an application of the course's
pure-visualization exemption. CPU mesh calculations and nearest-hit selection
are tested independently of upload/draw calls.

No numerical whole-project coverage is claimed. For optional line measurement
of the linked C++ modules, install the x64
[OpenCppCoverage 0.9.9.0 tool](https://github.com/OpenCppCoverage/OpenCppCoverage/releases/tag/release-0.9.9.0)
separately, build Debug (including PDB files), then run:

```powershell
.\scripts\coverage.ps1
# Or pass the tool's actual installed executable path:
.\scripts\coverage.ps1 -OpenCppCoverage "C:\tools\OpenCppCoverage\OpenCppCoverage.exe"
```

The script writes HTML and Cobertura reports under `coverage/` and filters to
`src/` in the unit-test executable. Modules absent from that executable are absent
from the denominator. A high percentage there must never be presented as coverage
of all application functionality. The tool is archived upstream and optional;
it is not downloaded or installed by this repository. The recorded validation
states whether coverage was actually measured.

## Continuous integration

[The workflow](../.github/workflows/sac4ap.yml) configures clean Windows 2022
runners, Python 3.12 and CMake 3.31.6, then builds and tests Debug and Release and
compiles the original application. It runs on pushes/pull requests targeting
`sac4ap` and can be dispatched manually. Full Git history is fetched for the
baseline comparison. JUnit and CTest logs are retained as workflow artifacts.

CI does not run the graphics smoke check: a compiler runner is not a guarantee
of an OpenGL 3.3 desktop. Before an exam demonstration, perform the manual
checklist on the intended machine and record the outcomes.