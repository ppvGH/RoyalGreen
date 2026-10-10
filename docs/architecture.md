# Architecture

[Documentation index](index.md) · [API](api.md)

| Module | Responsibility |
| --- | --- |
| [main](../src/main.cpp) | Parse settings, resolve assets, create context/UI, route frames and release owners in order |
| [runtime options](../src/core/runtime_options.h) / [Path](../src/app/path.h) | Configuration, executable discovery and runtime resource paths |
| [InputManager](../src/core/input_manager.h) / [ActionMap](../src/core/action_map.h) | Poll states once per frame and expose logical actions |
| [geometry](../src/core/geometry.h) / [collision](../src/core/collision.h) | Object-coordinate ray picking and strict 2D box overlap |
| [graphics](../src/graphics/) | OpenGL owners, camera, model import, meshes and materials |
| [Scene](../src/app/scene3D/scene.h) | Room/cabinet/lights, picking, camera transitions and rendering |
| [GameSession](../src/app/game2D/game_session.h) | CPU encounter, fixed bounds, pause/death/reset |
| [Game](../src/app/game2D/game.h) | Render a session into the cabinet framebuffer |
| [UIManager](../src/app/ui_manager.h) | ImGui menus, helpers and input gating |

## Startup, frames and state

Startup validates settings/resolves assets before window creation. A current context and loaded GLAD functions are required before ImGui/GPU objects. Failed resource loads throw without inserting placeholders. Models are imported, static point-light shadow maps rendered once, and a 600 x 600 game target created.

Frames poll events, compute elapsed seconds (capped at 0.25 after stalls), refresh framebuffer/projection and sample input. Menu state gates exploration/game input. ImGui draws controls; a powered display updates/renders the active encounter. Its texture feeds the cabinet; the room/UI render before swapping. Minimized windows wait instead of drawing a zero viewport.

Scene exploration/game flags are mutually exclusive when enabled. Play starts the two-stage approach; ordinary movement is suppressed during transitions. Exit game resets and animates back. Input gating uses retained menu state from the previous UI frame; full UI transitions remain manually checked.

GameSession owns actors/projectiles/collision coordination. `input()` handles menu/help and commands; `update()` moves/animates, clamps quads into the arena, removes spent/out-of-bounds shots, checks collisions and opens the terminal menu. View offset is always zero. Pause stops progression; reset restores bounds, timers/combat flags, lives and clips immediately, clears shots and preserves help preference.

## Timing and numerical contracts

Camera translation is scene units/second times elapsed seconds. Mouse offsets use radians/cursor unit. Logical window dimensions center cursor/UI; physical framebuffer dimensions drive viewport/perspective, including high DPI.

Animation consumes complete intervals and retains remainder. Idle/walk/jump loop; combat clips stop at the final frame. Crossed-frame queries preserve shoot events; immediate state/clip refresh permits consecutive attacks.

Room coordinates use Y up. Picking inverse-transforms origin (w=1) and direction (w=0), choosing the smallest accepted parameter. Triangle caches own snapshots; rebuild after geometry edits. Zero-area imported faces are omitted from picking without changing draw indices/assets. Tolerance is 1e-6, both faces hit and very small negative parameters are accepted. A non-unit ray returns a parameter, not an inherent distance.

2D collision is strict overlap; edge/corner contact is false. Inactive projectiles neither move nor hit again. No swept collision/room physics is added: fast shots can cross a target between updates.

## Resource lifetime and error handling

Texture, Shader and Mesh are move-only owners. Framebuffer, SpriteRenderer, Scene and UI destructors release allocations/backends. ResourceManager owns named maps; sprites/materials borrow entries. Do not clear/replace live borrowed entries; model mesh lookup pointers need stable vector storage after import.

Main cleanup order is game/scene, ImGui, maps, window, GLFW. The context stays current through GPU destruction and exception exits. Failed shader reload retains the old linked program and releases partial allocations. Resource/window failures print a diagnostic and exit nonzero. Handle-deletion checks do not measure whole-program memory/performance.

GameSession allocates no GPU objects and draws nothing, but actor construction still requires named textures. CPU tests explicitly inject ID-zero textures into ResourceManager. This seam permits deterministic coordination tests without being a fully independent domain library.
