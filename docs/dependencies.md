# Dependencies and reproducibility

[Documentation index](index.md) · [Installation](installation.md)

The [original root CMakeLists.txt](../CMakeLists.txt) selects the application's dependencies. The assessment work uses a separate test project and scripts; it does not require changing these production selections.

## Application libraries

| Dependency | Selection in this repository | Purpose / acquisition |
| --- | --- | --- |
| [GLFW](https://github.com/glfw/glfw/tree/3.4) | Git tag `3.4` | Window, OpenGL context, and keyboard/mouse input; CMake FetchContent. |
| [Dear ImGui](https://github.com/ocornut/imgui/tree/v1.91.9) | Git tag `v1.91.9` | Menus, help/FPS overlays; fetched source compiled with its GLFW and OpenGL3 backends into `imgui_lib`. |
| [Assimp](https://github.com/assimp/assimp/tree/v6.0.2) | Tag archive `v6.0.2` | OBJ model/material import; CMake FetchContent archive download. |
| [stb](https://github.com/nothings/stb) | Git branch `master` | Image loading through stb_image; FetchContent, header implementation in the project. |
| [GLM](https://github.com/g-truc/glm/tree/0.9.9.8) | Git tag `0.9.9.8` | Vectors/matrices and geometry; header-only, FetchContent. |
| [GLAD](https://github.com/Dav1dde/glad) | Checked-in generated loader, header identifies generator `0.1.36` | `external/glad/src/glad.c` and headers; generated for OpenGL 3.3. The generated header lists a compatibility profile; the application requests a Core context. |
| OpenGL | System package found by `find_package(OpenGL REQUIRED)` | Driver/system graphics API; app requests version 3.3 Core. |

Assimp is configured with its tests, samples, tools, and install disabled, all importers disabled by default, and the OBJ importer enabled. OBJ/GLTF exporters are explicitly disabled. The application's documented asset format is OBJ with MTL plus PNG textures and GLSL shader files; do not infer arbitrary-format support from Assimp's broader capabilities.

## Build and test tools

The application uses C++17 and the Visual Studio 2022/v143 Windows route. Use CMake 3.24 to a 3.x version. The local reference tool is 3.31.6-msvc6; the old GLM CMake policy setup is incompatible with unadjusted CMake 4.x configuration.

The [separate test CMake project](../tests/CMakeLists.txt) fetches Catch2 `v3.7.1` for C++ test assertions/discovery. It also fetches the matching GLM/GLFW/ImGui selections for headers without building the GLFW/ImGui runtime libraries or using GLM's old project policy setup. Tests provide fake GLFW input functions; they do not create a graphics context.

CTest is supplied with CMake. Python 3.10+ runs the repository verification/tests using standard-library modules; those scripts do not need a pip package install. See [testing](testing.md) for the separate test project's dependencies and actual coverage. The optional [smoke script](../scripts/smoke_app.ps1) needs a real graphics desktop. Optional [coverage](../scripts/coverage.ps1) requires separately installed legacy OpenCppCoverage 0.9.9.0 x64; no coverage percentage has been measured in the recorded setup. Optional [Doxygen configuration](../Doxyfile) requires Doxygen and has not been generated locally. These tools are not application runtime dependencies.

The unchanged code contains `_declspec(dllexport)` in [main.cpp](../src/main.cpp) and `std::atan2f`/`std::asinf` usage in [camera.cpp](../src/graphics/camera.cpp). This documentation promises the Windows/MSVC application workflow, not Linux/macOS application compatibility.

## Separate test dependencies

[tests/CMakeLists.txt](../tests/CMakeLists.txt) compiles Catch2 `v3.7.1` and
fetches headers for the same GLFW `3.4`, ImGui `v1.91.9` and GLM `0.9.9.8`.
The stb header is pinned in this new test project to immutable commit
`2c980bb59875b0d32144a71867fbdebb2f77cd20`. Header-only population does not build
GLFW or ImGui. The original GLAD source supplies graphics symbols; the unit
fixtures do not load GL functions or issue GPU calls. Only the two external
GLFW polling functions are replaced for deterministic input samples.

Catch2 assertions remain enabled in Debug and Release. Python verification uses
only the standard library. Neither optional coverage nor Doxygen is required by
the automatic suite. The application dependency choices above remain unchanged.
## Reproducibility limits

Version tags make most selections identifiable, but Git tags are not immutable commit pins. The stb `master` branch is a moving dependency, so separate fresh builds may retrieve different commits. The Assimp archive has no content hash declared in the root build. A successful build should record the exact fetched revisions/tool versions as well as the RoyalGreen commit.

Fresh configuration requires upstream network access; the repository does not contain every dependency. CMake caches downloads/sources in the build tree, which may let an existing tree work offline, but offline fresh configuration is not promised.

Resource paths embed the absolute clone location. Rebuilding in a new clone makes the demo runnable there; moving a previously built executable to another machine does not produce a self-contained installation. Platform, driver, and dependency differences can affect rendering or build results.

Preserving the application source/build logic also preserves these limitations. A future dependency-pinning or packaging change should be separately reviewed and validated.

## Licenses and asset provenance

This documentation does not choose or grant a new license for RoyalGreen. No project-wide license should be inferred from a dependency's license or a public repository. Consult the license/notices distributed with the exact fetched dependency revisions and the checked-in GLAD/Khronos files before redistribution.

The room models are described by the original project as created in Blender. That does not establish the redistribution rights of every texture, sprite sheet, or other asset. Record provenance and permissions separately where available; do not invent missing attribution or licensing.
