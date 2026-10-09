# RoyalGreen documentation

[Repository home](../README.md)

RoyalGreen combines a 3D arcade room with a 2D game displayed on a cabinet. It is written in C++17 and requests an OpenGL 3.3 Core context. This English documentation describes the implementation in the `sac4ap` branch, including its existing defaults and limitations.

## Start here

| Your task | Guide |
| --- | --- |
| Build and run on a Windows machine | [Installation](installation.md) |
| Explore the room and play the game | [Tutorial and controls](tutorial.md) |
| Understand modules, state, rendering, and resources | [Architecture](architecture.md) |
| Look up interfaces, units, preconditions, and examples | [API reference](api.md) |
| Run the automated checks and understand their scope | [Automated testing](testing.md) |
| Exercise the interactive application and record results | [Manual testing](manual-testing.md) |
| Identify libraries, versions, and build constraints | [Dependencies](dependencies.md) |
| Use smoke, coverage, and generated API tools | [Optional tools](installation.md#optional-smoke-coverage-and-api-documentation) |
| Maintain source, documentation, and history | [Contributing](contributing.md) |

The [improvement report](improvement-report.md), [exam readiness review](exam-readiness.md), and [validation record](validation.md) describe the assessment work and its evidence. Consult the records before treating a build, a CI job, or a manual scenario as passed.

## What the application exposes

The room supports mouse look, keyboard movement, a camera-height toggle, aiming and help overlays, and cabinet selection. The cabinet menu starts the game or returns to exploration. The game supports movement, jumping, shooting, enemy attacks, life indicators, scrolling, pause/resume, reset, and return to the room. Escape and the window close button end the application.

There are no application command-line options or user configuration files. A standard demo needs no source edits. [Tutorial defaults](tutorial.md#defaults-and-configuration) identifies the fixed settings; [API reference](api.md) explains the processing behind the interactive features.

## Supported demonstration route

Build locally on Windows 10/11 x64 with Visual Studio 2022/v143 and CMake 3.24 to 3.x; keep the clone and resources at their configured location. Linux and macOS application builds are not promised by this documentation. The original source contains Microsoft-specific declarations and math calls.

The root `CMakeLists.txt` builds the application. The independent `tests/CMakeLists.txt` builds the regression tests. Static asset checks, unit tests, application builds, and interactive checks answer different questions; passing one does not establish that the others passed.

## Documentation and evidence conventions

Code links point to the actual source. API examples are small fragments for use with the repository headers and dependencies, not command-line modes of the application. Manual checklists specify expected outcomes and provide blank result fields; they do not claim that the scenarios were executed.

Historical Git commits retain their original dates and content. New work should have new commits with its actual dates. Current assessment additions should not be presented as activity in an earlier year.
