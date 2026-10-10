# RoyalGreen portable package

Extract the whole ZIP and run `royalgreen.exe` on Windows x64 with an OpenGL 3.3
Core driver. Keep `res/` beside the executable. No Visual Studio, CMake or Python
is needed. The default Windows build embeds the C/C++ runtime.

WASD/mouse explore; H shows help; F toggles fixed height; T toggles aiming.
Aim at the cabinet, left-click and choose Play. A/D move, W jumps, S shoots,
M opens the game menu. The arena is fixed; neither the view nor the player can
scroll off the screen. Esc closes the application.

Run `royalgreen.exe --help` for resolution, asset-directory and configuration
options. `--config FILE` reads key=value settings; command-line settings win.
`royalgreen.exe --width 1280 --height 720 --camera-speed 1.35` is an example.

Linux and macOS require their own source builds; a Windows EXE is not a native
application on those platforms. See the repository's installation guide.
Third-party library notices are in `third-party/`. Asset provenance and the
project-license decision are recorded in `asset-provenance.md`.
