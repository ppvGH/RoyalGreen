# Tutorial and controls

[Documentation index](index.md) · [Installation](installation.md)

## A complete first session

1. Build and launch `royalgreen.exe` using the [installation guide](installation.md). The initial view is the arcade room, with the cabinet screen off, an aiming cross, a helper overlay, and an FPS overlay.
2. Move the mouse to look around. Hold **W/A/S/D** to move. The mouse is captured during exploration; opening a menu releases it.
3. Press **F** to toggle fixed camera height. With it disabled, forward/backward movement follows the viewing direction, including its vertical component. The height toggle is not a collision or walking-physics system.
4. Press **T** to hide/show the aiming cross and **H** to hide/show help. Look toward the arcade cabinet; use the center of the window as your aim.
5. Left-click while aiming at the cabinet. The **Arcade** menu should appear. Choose **Exit** to dismiss this menu and keep exploring, or **Play** to start.
6. After **Play**, wait for the camera approach animation, configured as a two-second first stage and a one-second second stage. The game appears on the cabinet display; 2D controls become active after the approach finishes.
7. Hold **A/D** to move left/right. Press **W** to jump, then land. Press **S** while grounded to shoot an arrow in the direction the player faces. Shooting temporarily prevents movement; hold movement keys rather than expecting a brief tap to move continuously.
8. Avoid the cat's energy balls. Successful arrows reduce its displayed lives; the cat starts with nine. A projectile hit on the player leads to its hit/death sequence. Combat and timing have existing limitations, so use the reset menu whenever needed.
9. Press **M** to open the **Game Menu**. Test **Return to game**, then reopen the menu and choose **Reset game**. Reset restores the player and cat positions, nine cat lives, clears projectiles, and resets the scrolling camera.
10. Choose **Exit game** to reset the game, turn off the cabinet screen, and animate back to the room. Use **Esc** or the window close button when finished.

Selection tests the arcade model's triangles, not every object in the room. A click away from the cabinet should not open its menu. There is no selection-distance limit or general room-occlusion test in the implementation.

## Controls

| Context | Input | Effect |
| --- | --- | --- |
| 3D room | W / A / S / D held | Forward / left / backward / right movement |
| 3D room | Mouse movement | Change viewing direction |
| 3D room | F pressed | Toggle fixed camera height |
| 3D room | T pressed | Show/hide the aiming cross |
| 3D room | H pressed | Show/hide the 3D helper |
| 3D room | Left mouse click | Select the cabinet along the viewing ray |
| Arcade menu | Play | Turn on the screen and approach the game |
| Arcade menu | Exit | Close this menu and return to exploration |
| 2D game | A / D held | Move left / right |
| 2D game | W pressed | Jump when permitted |
| 2D game | S pressed | Start a grounded shooting action |
| 2D game | H pressed | Show/hide the 2D helper |
| 2D game | M pressed | Open the game menu |
| Game menu | Return to game | Resume |
| Game menu | Reset game | Reset the current game and resume |
| Game menu | Exit game | Reset the game and return to the room |
| Any application state | Esc / window close | Close the application |

These bindings come from [main.cpp](../src/main.cpp), the menus from [ui_manager.cpp](../src/app/ui_manager.cpp), and Escape from [callbacks.cpp](../src/core/callbacks.cpp). Registered debugging keys or unbound actions in source are not additional playable controls.

## Gameplay behavior

The game is a single-player encounter with one autonomous cat enemy. It has scrolling, animated states, projectiles, and a life-icon HUD for the cat. Movement, gravity, and projectile updates use frame elapsed time in seconds. The cat attacks automatically and can use a jumping barrage; the player has no displayed multi-life counter.

Opening a menu pauses the main game update. The menu opens automatically when the game update observes a player or cat in the `Dead` state. This is not a separate victory/defeat screen. Choose **Reset game** to start another encounter or **Exit game** to explore again.

## Defaults and configuration

The standard demo needs no file edits. The entry point is `int main()`: it does not parse command-line arguments. There is no application configuration-file loader, key-rebinding UI, save system, or runtime settings menu.

| Setting | Current default | Source |
| --- | --- | --- |
| Window | 70% of primary monitor dimensions; initial position (400, 200) | [main.cpp](../src/main.cpp), [utilities.cpp](../src/core/utilities.cpp) |
| Graphics context | OpenGL 3.3 Core, double buffering | [utilities.cpp](../src/core/utilities.cpp) |
| Game render target | 600 x 600 | [main.cpp](../src/main.cpp) |
| Camera start | (2.0, 1.7, 0.0); fixed height initially off | [scene_data.h](../src/app/scene3D/scene_data.h) |
| Camera projection | 45-degree vertical field of view; near 0.1, far 20; aspect set from initial framebuffer size | [camera.cpp](../src/graphics/camera.cpp), [scene.cpp](../src/app/scene3D/scene.cpp) |
| Camera sensitivities | Mouse 0.001; keyboard 0.15 | [scene_data.h](../src/app/scene3D/scene_data.h) |
| Shadow maps | 1024 x 1024 cube faces | [scene_data.h](../src/app/scene3D/scene_data.h) |
| Helpers / aiming cross | Initially visible | [scene.cpp](../src/app/scene3D/scene.cpp), [game.cpp](../src/app/game2D/game.cpp) |
| Game ground / gravity | Ground y = 100; acceleration = 1200 game units/s² | [game_data.h](../src/app/game2D/game_data.h) |
| Player | Start (0, 100), sprite 100 x 100, walk speed 200 units/s, jump speed 600 units/s | [game_data.h](../src/app/game2D/game_data.h) |
| Cat | Start (500, 100), sprite 100 x 100, nine lives | [game_data.h](../src/app/game2D/game_data.h) |
| Arrow / ordinary energy-ball speed | 600 game units/s | [game_data.h](../src/app/game2D/game_data.h) |
| Sprite grids | Six rows, thirteen columns; animation indices start at one | [game_data.h](../src/app/game2D/game_data.h), [animator.cpp](../src/app/game2D/animator.cpp) |
| Resource directories | Absolute paths under the configured clone's `res/` | [CMakeLists.txt](../CMakeLists.txt), [path.h](../src/app/path.h) |

The 2D units are coordinates in the game render target, not real-world metres. Mouse sensitivity multiplies mouse offsets into angles in radians. The 3D movement code uses a per-call displacement and is not scaled by frame time in the application; do not interpret keyboard sensitivity as metres per second.

Editing these constants is development work and requires rebuilding. It is not part of the normal demo setup.

## Limitations to expect

The camera can pass through room objects; there is no 3D collision system. Game collisions cover projectiles versus their opposing character, not general character/environment or character/character collisions. Resizing updates the OpenGL viewport but does not consistently update all stored dimensions, projection settings, and UI positions.

There is no save/load, score table, multiplayer, or runtime asset-selection interface. Menu/animation transitions and combat state changes should be exercised using the [manual checklist](manual-testing.md). Treat a reset or death behavior that differs from the checklist as a finding to record, not a result to silently mark passed.
