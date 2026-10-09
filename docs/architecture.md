# Architecture

[Documentation index](index.md) · [API reference](api.md)

## Modules and responsibilities

| Area | Responsibility | Representative source |
| --- | --- | --- |
| Application entry point | Create the window and UI, load assets, bind inputs, construct scene/game, and coordinate each frame | [main.cpp](../src/main.cpp) |
| Core input | Poll registered keys/buttons and map them to logical actions | [input_manager.h](../src/core/input_manager.h), [action_map.h](../src/core/action_map.h), [callbacks.cpp](../src/core/callbacks.cpp) |
| Core resources | Load shader text and PNG textures into named static maps | [resource_manager.cpp](../src/core/resource_manager.cpp), [path.h](../src/app/path.h) |
| Core geometry/collision | Ray/triangle intersection and strict 2D bounding-box overlap | [geometry.cpp](../src/core/geometry.cpp), [collision.cpp](../src/core/collision.cpp) |
| Graphics | Shader programs, textures, framebuffers, camera matrices, imported models, meshes, and materials | [graphics/](../src/graphics/) |
| 3D scene | Room, cabinet, pool table, lamps, shadows, picking, and camera transitions | [scene.cpp](../src/app/scene3D/scene.cpp), [arcade.cpp](../src/app/scene3D/arcade.cpp), [room.cpp](../src/app/scene3D/room.cpp) |
| 2D game | Character state, animation, body movement, projectiles, collisions, scrolling, and HUD | [game.cpp](../src/app/game2D/game.cpp), [player.cpp](../src/app/game2D/player.cpp), [cat.cpp](../src/app/game2D/cat.cpp) |
| UI | Cabinet/game menus, helpers, FPS overlay, and menu input gating | [ui_manager.cpp](../src/app/ui_manager.cpp) |
| Tests and checks | Separate regression project plus asset/preservation scripts | [Testing](testing.md) |

`sceneData` and `gameData` hold compile-time defaults. `Path` names resource files. The source hierarchy separates concerns, but it is not a strict dependency-layer architecture: core geometry and graphics use application constants, and several gameplay constructors depend on already-loaded graphics resources.

## Startup and frame sequence

Startup requests an OpenGL 3.3 Core window at 70% of the primary monitor's dimensions, creates the ImGui integration, loads OpenGL entry points, enables depth testing and alpha blending, loads resources, and creates the two input maps. The scene imports its models and initializes three point-light depth cube framebuffers. A one-time depth pass builds shadows for the static room and lights. The game creates a 600 x 600 color framebuffer.

Each frame in [main.cpp](../src/main.cpp) performs these operations:

1. Poll GLFW events, compute elapsed time, and update registered input states.
2. Route inputs to exploration or gameplay when menus allow it; use a normal cursor for menus and a captured cursor otherwise.
3. Begin the ImGui frame and draw menu/helper widgets, which update menu flags and may change scene/game state.
4. If the cabinet display is on, update the game unless a menu is open, then render it into its framebuffer.
5. Assign that framebuffer's texture to the cabinet display material.
6. Update camera transition animations and draw the 3D room to the window.
7. Render ImGui over the scene and swap the window buffers.

Input routing reads the menu state retained from the previous UI frame before drawing the current one. This ordering matters when checking transition edges; it should not be documented as a formally verified state machine.

## State and transitions

`Scene` stores exploration/game input flags, the display state through `Arcade`, camera-animation flags, aiming/help flags, and the cabinet-menu flag. `Game` stores its menu/help flags and gameplay objects. `UIManager` derives whether any menu is open as it draws the frame.

Enabling `setInput3D(true)` disables 2D input, and enabling `setInput2D(true)` disables 3D input. Both can be disabled, such as during a transition or a menu. Selecting the cabinet opens its menu; **Play** turns on the display and starts the approach. The two-stage approach interpolates position and front direction, then enables 2D input. **Exit game** resets gameplay, switches the display off, and starts the return animation. The source enables 3D input at the start of the return but suppresses camera movement while an animation remains active.

Player and cat behavior use `State` values `Idle`, `Walk`, `Jump`, `Attack`, `Hit`, and `Dead`. State changes select animation data. Attacks emit projectiles at a configured frame. A projectile marks its opposing character as hit; cat hits reduce lives, while a player hit leads to death. The main game update opens its menu when it observes either character dead.

`resetGame()` calls the existing player, cat, and projectile resets and resets the scrolling camera. It does not reconstruct the full scene or reset help preferences. Some flags and bounding boxes are not immediately refreshed by the reset methods, so full state equivalence requires tests and manual observation.

## Rendering and asset processing

Assimp imports OBJ files with triangulation and joining of identical vertices. Materials are read before meshes so that named mesh/material relationships can be built. `Mesh::setup()` creates VAO/VBO/EBO objects. The CPU vertex data also supplies cached `TriangleData` used for picking.

The room uses a Blinn-Phong shader with material uniforms, a tiled floor texture, and three point-light shadow maps. Each shadow cube map contains six faces of 1024 x 1024. The cabinet display uses the game framebuffer texture and a CRT shader while powered on; its power-button material changes emission. Spot-depth helpers and shaders exist in source, but the active scene depth pass uses point lights.

The 2D renderer draws textured quads using an orthographic projection. Character sprites select UV rectangles using `Animator`; projectiles and HUD use textures. The scrolling camera is a horizontal offset. The game renders before the room so that the cabinet samples the current game texture.

Resource filenames are compiled from the configured clone's absolute path. There is no runtime asset search or configuration loader. OBJ normals, named cabinet/floor/lamp meshes, MTL relationships, PNG integrity, and sprite-grid dimensions are part of the asset contracts.

## Coordinate and numerical contracts

The room uses a world coordinate system with Y up. Imported mesh triangles live in object coordinates. Cabinet picking applies the inverse model matrix to the camera origin with homogeneous `w = 1` and to its direction with `w = 0`, then searches for the smallest accepted ray parameter across the cabinet's triangles.

The intersection uses a tolerance of `1e-6`, accepts both triangle faces, and allows small negative parameters within tolerance. The returned value is `t` in `origin + t * direction`; it is not inherently a world-space distance when the direction is not unit length.

2D bounding boxes are axis-aligned rectangles from lower-left position plus size. Collision uses strict overlap on both axes; touching an edge or corner is not a hit. There is no swept collision, so a fast projectile can cross a target between updates.

Animation rows and columns are one-based after `setAnimation()`. Updates advance at most one frame per call and discard excess elapsed time. This affects animation speed under long frames and should remain visible in the test/limitation discussion.

## Resource lifetime and error handling

The OpenGL context must exist before resource loading, model setup, framebuffer creation, rendering, and any GL deletion. Sprites reference textures in static resource maps; materials reference externally supplied textures; meshes hold shader pointers. Projectiles are owned by `unique_ptr` values in a vector. Triangle data references a vertex position, so the backing vertex storage must remain alive and stable.

The existing wrappers do not provide complete ownership safety. `Shader` deletes its program in its destructor but remains copyable; `Texture`, `Framebuffer`, and `Mesh` do not implement matching GL-object destruction. Static shader-map destruction occurs after `main` has destroyed the GLFW window/context. These are existing lifecycle limitations, not guarantees of leak-free cleanup.

Error handling is mixed: resource getters and missing mesh lookups throw, shader/texture/framebuffer failures often log and continue, and a failed model import can leave an empty model. Texture-load failure can leave a default ID-zero map entry. A successful process launch alone does not establish successful asset loading or rendering.

## Testing boundaries

Pure rendering calls need an actual graphics context and visual checks. Embedded processing still needs tests: model import/name contracts, triangle intersection and nearest-hit search, camera calculations, UV animation, sprite/body updates, projectile lifecycle, collision routing, reset behavior, and menu/input transitions.

The [automated coverage record](testing.md) identifies implemented tests and gaps. The [manual checklist](manual-testing.md) covers the interactive and visual behavior; it does not replace deterministic tests for testable processing. No claim of complete API coverage or full cross-platform application support follows from the current module structure.
