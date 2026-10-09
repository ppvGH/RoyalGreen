# Installation

[Documentation index](index.md) · [Next: tutorial](tutorial.md)

## Requirements

The documented application build targets **Windows 10/11 x64**.

| Requirement | Purpose and setup |
| --- | --- |
| Visual Studio 2022 or its Build Tools | Install **Desktop development with C++**, the **MSVC v143** x64/x86 tools, and a Windows 10/11 SDK. |
| CMake **3.24 <= version < 4** | Use a 3.x release for the unchanged dependency setup. The local reference tool is **3.31.6-msvc6**. |
| Git | Required to clone RoyalGreen and to fetch several dependencies during configuration. |
| OpenGL 3.3 Core graphics support | Install the appropriate GPU driver; the app needs a working graphical desktop and OpenGL context. |
| Internet access on first configuration | CMake retrieves GLFW, ImGui, Assimp, stb, and GLM. |
| Python 3.10+ for checks | The verification scripts use the standard library; Python is not required to run the application. |

The local application configuration was confirmed with CMake 3.31.6-msvc6, Visual Studio 2022, MSVC 19.44, and Windows SDK 10.0.26100. Configuration success is separate from compilation and interactive validation; see the [validation record](validation.md) for completed results.

The root project declares CMake 3.15, but that declaration alone does not describe the requirements of the current dependencies and documented workflow. The pinned GLM 0.9.9.8 has a CMake policy minimum of 3.2, whereas [CMake 4.0 removed compatibility below 3.5](https://cmake.org/cmake/help/latest/release/4.0.html). Using a 3.x tool avoids changing production build files. The [Visual Studio 2022 generator](https://cmake.org/cmake/help/latest/generator/Visual%20Studio%2017%202022.html) selects v143 by default and accepts `-A x64`.

## Prepare the tools

Open PowerShell and check:

```powershell
git --version
cmake --version
```

If CMake is not on PATH, use the compatible bundled tool installed with Visual Studio. For a default Community installation, the following is an example; adjust the edition/path to your installation:

```powershell
$cmakeBin = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin"
$env:Path = "$cmakeBin;$env:Path"
cmake --version
```

This changes PATH only in that PowerShell session. A Visual Studio developer terminal is another way to obtain its configured tools.

## Clone and build the original application

Choose a writable parent directory. The destination does not need to be `C:\dev\RoyalGreen`.

```powershell
git clone --branch sac4ap https://github.com/ppvGH/RoyalGreen.git
Set-Location RoyalGreen
git branch --show-current
cmake -S . -B build/app -G "Visual Studio 17 2022" -A x64
cmake --build build/app --config Debug --target royalgreen --parallel
.\build\app\Debug\royalgreen.exe
```

The branch check should print `sac4ap`. Configuration should generate Visual Studio project files, and a successful build should produce `build/app/Debug/royalgreen.exe`. Downloads and warnings from dependency CMake files are not themselves build failures; inspect the final result and any actual errors.

For Release, reuse the same generated build directory:

```powershell
cmake --build build/app --config Release --target royalgreen --parallel
.\build\app\Release\royalgreen.exe
```

This generator supports multiple configurations, so select Debug or Release with `--config`. These commands use the original root `CMakeLists.txt`. The repository also has Visual Studio folder settings in `CMakeSettings.json`; their Ninja output paths differ from the explicit commands above.

## Resources and installation limits

The root build defines `SHADERS_DIR`, `MESHES_DIR`, and `TEXTURE_DIR` from `CMAKE_SOURCE_DIR`. [Asset paths](../src/app/path.h) append filenames to those absolute directories. A locally built executable therefore reads resources from the clone used to configure it, even when started from another working directory.

Keep the entire `res/` directory at that location. Copying the executable and resources into a different folder does not update the compiled paths. After moving the source to a new location, configure a fresh build directory there and rebuild.

The existing install command is:

```powershell
cmake --install build/app --config Release --prefix install
```

The RoyalGreen install rule installs its executable to `install/bin`. It does not install the application's resources, rewrite their paths, or assemble all runtime dependencies. Dependency install rules may also run. Treat this as an install-rule demonstration, not a standalone distribution.

Older prebuilt releases may contain the original author's build paths. The former `C:\dev\RoyalGreen\res` instructions were specific to such a binary. A fresh local build is the documented route on another machine.

## Configure and run the separate tests

From the clone root:

```powershell
python --version
python scripts/verify_assets.py
python scripts/check_source_unchanged.py
cmake -S tests -B build/tests -G "Visual Studio 17 2022" -A x64
cmake --build build/tests --config Debug --parallel
ctest --test-dir build/tests -C Debug --output-on-failure
```

The application and tests have separate build directories. The [asset verifier](../scripts/verify_assets.py) checks referenced files, OBJ/MTL relationships and normals, GLSL version declarations, PNG chunk checksums, and sprite-grid dimensions. It does not compile shaders or establish visual correctness.

The [preservation script](../scripts/check_source_unchanged.py) defaults to the immutable original commit `20ae12414fb991aa6d0e4b172df25e5239fe81b8`; `--base main` explicitly compares against the current local main instead. Both verification scripts find their repository from their own location and can be invoked from another working directory using their full paths.

The [PowerShell helper](../scripts/run_checks.ps1) runs the repository checks:

```powershell
.\scripts\run_checks.ps1 -Configuration Debug
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
```

The helper validates assets and source preservation and runs the Python and C++ tests through CTest. `-BuildApplication` additionally builds the original app. Use `-Jobs 2` to limit parallel builds/tests or `-Python` to select a Python executable. Check [automated testing](testing.md) for the exact coverage and options.

## Optional smoke, coverage, and API documentation

After building the selected application configuration, run the [desktop smoke check](../scripts/smoke_app.ps1):

```powershell
.\scripts\smoke_app.ps1 -Configuration Debug
.\scripts\smoke_app.ps1 -Configuration Release
```

This requires a graphical Windows session and OpenGL 3.3. It waits for startup, closes the application window, and checks renderer/error diagnostics and exit status. Logs are written under `build/smoke`. Its default observation interval is eight seconds; `-DurationSeconds` accepts 2 to 60. It does not exercise controls, combat, menus, or assert visual correctness. Completed smoke results are recorded in [validation](validation.md).

[coverage.ps1](../scripts/coverage.ps1) is optional and requires a separately installed OpenCppCoverage 0.9.9.0 x64. The script describes this as a legacy tool archived upstream. It has not been installed or used to measure a percentage in the recorded local setup. After a Debug test build, `.\scripts\coverage.ps1` produces HTML under `coverage/cpp-html` and Cobertura XML at `coverage/cpp-cobertura.xml` if that tool is available; `-OpenCppCoverage` selects its executable. Reports concern only production modules linked into the test binary, not the whole application.

The optional [Doxyfile](../Doxyfile) generates browsable API HTML:

```powershell
doxygen Doxyfile
```

Run from the clone root with Doxygen installed; output is `docs/html/index.html`, with warnings logged to `build/doxygen-warnings.log`. Doxygen is unavailable in the recorded local setup, so generation is unverified. The written [API reference](api.md) remains available without this tool. Do not commit generated HTML as a substitute for maintained English documentation.

## Run on another machine

Clone `sac4ap` on a second Windows machine with the prerequisites above, configure and build there, then follow the [tutorial](tutorial.md) and [manual checklist](manual-testing.md). Preserve the clone path while running. Record the OS, compiler, SDK, CMake version, GPU/driver, commit, and results. A second-machine run remains unverified until that record exists.

## Troubleshooting

| Symptom | Action |
| --- | --- |
| CMake reports removal of old-version compatibility | Check `cmake --version`; use CMake 3.24 to 3.x for this workflow. |
| Visual Studio generator or compiler cannot be found | Install the C++ workload, v143 tools, and SDK; check the selected VS 2022 installation. |
| Existing build cache uses another generator or source path | Choose a new build directory, for example `build/app-vs2022`, and adjust later paths to match. |
| FetchContent clone/download failure | Verify Git, internet, proxy/TLS settings, and upstream access, then rerun configuration. No dependencies are fully vendored. |
| Build exhausts memory | Replace `--parallel` with `--parallel 2` or `--parallel 1`. |
| Textures, models, or shaders fail to load | Keep the configured clone and `res/` intact; run the asset verifier; rebuild after relocating the clone. |
| GLFW/GLAD startup failure or no rendered room | Check GPU driver and OpenGL 3.3 support in the actual graphical session; capture the console diagnostics. |
| UI or image appears distorted after resizing | This is a known resize limitation; relaunch at the initial window size and record it. |
| CTest cannot find a test executable | Build `build/tests` with the same configuration passed to CTest, e.g. Debug with `-C Debug`. |
