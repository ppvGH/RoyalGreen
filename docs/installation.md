# Installation and builds

[Documentation index](index.md) · [Tutorial](tutorial.md) · [Testing](testing.md)

## Windows x64 package

Extract the whole ZIP into a writable directory and run `royalgreen.exe`. Keep `res/`, the example configuration and notices with it. The default Release build uses static libraries and the static MSVC C/C++ runtime; Visual Studio, CMake and Python are build tools, not package runtime requirements.

The target is Windows 10/11 **x64** on Intel/AMD PCs with an OpenGL 3.3 Core graphics driver and a graphical desktop. Integrated or dedicated GPUs can satisfy that requirement; see the [GLFW driver explanation](https://www.glfw.org/faq.html#15---what-versions-of-opengl-are-supported-by-glfw). Local validation used Windows 11 and an RTX 4060 Laptop GPU. A Windows x64 EXE is not a native Linux/macOS/ARM executable. Windows ARM emulation, virtual desktops and Wine are outside the verified package target.

Double-click starts the game. Use PowerShell to retain diagnostics:

```powershell
.\royalgreen.exe --width 1280 --height 720
.\royalgreen.exe --help
.\royalgreen.exe --graphics-check
```

`--help` needs neither assets nor a graphics context. `--graphics-check` loads real assets, renders a frame and checks GPU/resource lifetime, then exits. Normal startup prints the renderer. Failures print `RoyalGreen: ...` and exit nonzero.

## Common source requirements

Use Git, a C++17 compiler, CMake **3.24 to 3.x** (reference **3.31.6**) and network access for the first configure. Python 3.10+ runs tests/package verification. Library revisions are [pinned](dependencies.md). Downloads are cached in build directories; fresh offline builds are not promised.

```sh
git clone --branch sac4ap-dev https://github.com/ppvGH/RoyalGreen.git
cd RoyalGreen
```

CMake copies `res/` beside the generated binary. Installed binaries search their own location independently of the working directory. `--assets PATH` provides an explicit alternative.

## Windows source build

Install Visual Studio 2022 Community/Build Tools with **Desktop development with C++**, MSVC v143 and a Windows SDK. Install Git, CMake 3.31.6 and Python with commands on PATH. In PowerShell:

```powershell
cmake -S . -B build/app -G "Visual Studio 17 2022" -A x64
cmake --build build/app --config Release --parallel 4
.\build\app\Release\royalgreen.exe
.\scripts\run_checks.ps1 -Configuration Debug -BuildApplication
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
.\scripts\package_release.ps1
```

The last command creates/verifies a ZIP, `.zip.sha256` and `.manifest.json` under `build/packages/`. It publishes no tag/release. It explicitly selects `ROYALGREEN_STATIC_RUNTIME=ON`; fetched libraries are static. Changing that setting requires reviewing/distributing runtime dependencies.

## Linux source build (Ubuntu 24.04 reference)

Linux uses X11, including through XWayland where available. Install compiler/OpenGL and the [GLFW X11 build requirements](https://www.glfw.org/docs/3.4/compile_guide.html#compile_deps_x11):

```sh
sudo apt-get update
sudo apt-get install build-essential git cmake python3 libgl1-mesa-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
cmake -S . -B build/app -DCMAKE_BUILD_TYPE=Release
cmake --build build/app --parallel 4
./build/app/royalgreen
cmake -S tests -B build/tests -DCMAKE_BUILD_TYPE=Release
cmake --build build/tests --parallel 4
ctest --test-dir build/tests --output-on-failure
```

Use the native compiler/display driver. The Windows ZIP is not used. Binary ABI compatibility across arbitrary Linux distributions is not claimed; build locally.

## macOS source build (Intel or Apple Silicon)

Install Xcode Command Line Tools (`xcode-select --install`), Git and CMake 3.31.6. Build with the native compiler/architecture; Python 3.10+ runs tests. The project requests a forward-compatible Core OpenGL context as [required by GLFW on macOS](https://www.glfw.org/faq.html#41---how-do-i-create-an-opengl-30-context).

```sh
cmake -S . -B build/app -DCMAKE_BUILD_TYPE=Release
cmake --build build/app --parallel 4
./build/app/royalgreen
cmake -S tests -B build/tests -DCMAKE_BUILD_TYPE=Release
cmake --build build/tests --parallel 4
ctest --test-dir build/tests --output-on-failure
```

Native Intel and ARM builds are distinct binaries. No macOS bundle, signature or notarized release is produced. CI compilation/CPU tests and hardware gameplay validation are tracked separately.

## Installation and relocation check

```sh
cmake --install build/app --config Release --prefix build/install
```

Launch `build/install/royalgreen` (Windows: `royalgreen.exe`) from another working directory. It must find `res/` beside itself. On Windows also test the ZIP extracted to a path with spaces: `--graphics-check`, a short normal `--exit-after 2` session and invalid `--assets`. See [validation](development-validation.md) for completed checks.

## Troubleshooting

| Symptom | Action |
| --- | --- |
| No compiler/generator | Install the platform compiler; Windows commands require VS 2022 x64 tools. |
| Dependency download fails | Check network and the upstream URL; retain immutable revision/hash. |
| Cannot find assets | Keep the entire `res/` beside the EXE or specify its root with `--assets`. |
| Cannot create OpenGL 3.3 window | Check graphics driver/desktop; compilation does not supply a driver. |
| Shader/image/model error | Retain filename/diagnostic; run `python scripts/verify_assets.py`. |
| Invalid option/config | Run `--help`; use only [documented keys](tutorial.md). |

## Optional smoke, coverage and API documentation

`--graphics-check` is the context integration mode. `--exit-after 2` runs normal startup/drawing/clean exit. `scripts/smoke_app.ps1` also provides a desktop startup check.

`doxygen Doxyfile` generates local API HTML under `docs/html/` when installed. `scripts/coverage.ps1` measures linked C++ modules using separately installed OpenCppCoverage. Neither is a runtime dependency. No whole-project coverage or generated HTML success is claimed without a recorded run.
