# RoyalGreen

[![sac4ap-dev verification](https://github.com/ppvGH/RoyalGreen/actions/workflows/sac4ap-dev.yml/badge.svg?branch=sac4ap-dev)](https://github.com/ppvGH/RoyalGreen/actions/workflows/sac4ap-dev.yml?query=branch%3Asac4ap-dev)

RoyalGreen is a C++17/OpenGL application: explore a 3D arcade room, select the cabinet, and play its embedded 2D encounter against a cat. Models, lighting, shadows, sprite animation, projectiles and menus form one working demonstration.

This **sac4ap-dev** branch implements the [approved improvements](docs/improvements-implemented.md) and fixes the arena view at its initial position. **main** retains the original project; **sac4ap** retains the comments, documentation and tests adaptation. All original commits remain ancestors of this branch; new work has real resumption dates.

## Run on Windows x64

Extract the entire Windows ZIP and run `royalgreen.exe`, keeping `res/` beside it. The package includes its C/C++ runtime and library dependencies. You need a Windows 10/11 x64 desktop and a graphics driver supporting **OpenGL 3.3 Core**. Intel/AMD CPUs are suitable; the GPU need not be NVIDIA. Windows ARM, Linux and macOS need their own builds; compatibility through emulation is not promised.

Build a package locally with Visual Studio 2022 C++ tools, Git, Python 3.10+ and CMake 3.31.6:

```powershell
git clone --branch sac4ap-dev https://github.com/ppvGH/RoyalGreen.git
cd RoyalGreen
.\scripts\package_release.ps1
```

The verified ZIP, SHA256 and asset manifest are under `build/packages/`. The script creates local artifacts; GitHub release publication is a separate step. [Installation](docs/installation.md) includes source build instructions for Windows, Linux and macOS, troubleshooting and relocation checks.

## Play and verify

WASD/mouse explore; left-click the cabinet and choose **Play**. In the game A/D move, W jumps, S shoots, M opens the menu, H toggles help. The view stays fixed and actors remain within the arena. Esc closes the application. See the [tutorial](docs/tutorial.md) for a complete session and configuration examples.

```powershell
.\scripts\run_checks.ps1 -Configuration Debug -BuildApplication
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
.\build\app\Release\royalgreen.exe --graphics-check
```

The CPU suite runs without a window; the last command requires a real OpenGL desktop. See [testing scope](docs/testing.md) and [development validation](docs/development-validation.md) for results and limits.

[Documentation](docs/index.md) contains architecture, API contracts, dependencies, manual checks and exam evidence. Asset attribution/project licensing still needs the owner's inventory; see [asset provenance](docs/asset-provenance.md).

## Preview

Historical repository screenshots illustrate appearance; they are not new validation evidence.

![The RoyalGreen arcade room](preview/panoramic.png)
![The 2D game on the arcade cabinet](preview/game1.png)
