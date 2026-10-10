# Dependencies and reproducibility

[Documentation index](index.md) · [Installation](installation.md)

The [dependency manifest](../cmake/dependencies.cmake) uses immutable commits and a checked archive. The test project selects matching headers and pinned Catch2. First configuration downloads dependencies into ignored build trees.

| Dependency | Exact revision / archive | Purpose |
| --- | --- | --- |
| GLFW 3.4 | `7b6aead9fb88b3623e3b3725ebb42670cbe4c579` | Context, window and input |
| Dear ImGui 1.91.9 | `97428e8ac99e339ce05eee531cf55b77b29ea709` | Menus/help/FPS overlays |
| stb | `2c980bb59875b0d32144a71867fbdebb2f77cd20` | PNG/JPEG decoding |
| GLM 0.9.9.8 | `bf71a834948186f4097caa076cd2663c69a10e1e` | Header-only vectors/matrices |
| Assimp 6.0.2 | `v6.0.2.tar.gz`, SHA256 `d1822d9a19c9205d6e8bc533bf897174ddb360ce504680f294170cc1d6319751` | OBJ/MTL import |
| Catch2 3.7.1 | `fa43b77429ba76c462b1898d6cd2f2d7a9416b14` | Test assertions/discovery |
| GLAD | Original checked-in 0.1.36 generated loader | OpenGL entry points |
| OpenGL | Platform system/driver, requested 3.3 Core | Graphics |

Assimp builds only the OBJ importer; samples/tools/tests/exporters are disabled. GLFW tests/examples/docs are disabled; Linux selects X11. GLM is populated as headers without its old CMake policy setup. All app dependency libraries are static. Windows defaults to `/MT` (`/MTd` for Debug) to embed the MSVC runtime.

Reference tools are C++17, CMake 3.31.6, Python 3.10+ for checks, VS 2022/v143 on Windows, native GCC/Clang on Linux and Apple Clang on macOS. System/compiler/driver versions still influence output; pins do not make builds byte-identical or ensure arbitrary binary compatibility.

The package includes notices for GLFW, ImGui, Assimp, stb, GLM, GLAD/Khronos and bundled zlib. [Asset provenance](asset-provenance.md) records the owner's project-license/asset inventory still pending. No project license is inferred from a dependency.

Doxygen/OpenCppCoverage are optional development tools. No generated API site or whole-project coverage percentage is claimed in the current validation record.
