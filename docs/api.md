# API reference

[Documentation index](index.md) · [Architecture](architecture.md) · [Testing](testing.md)

RoyalGreen is an executable, not an installed reusable SDK. These are its source interfaces. Include paths in the examples are relative to the `src/` include directory used by the application. Link the corresponding implementation files and dependencies when using a fragment.

There are no executable flags, HTTP endpoints, or external configuration schemas. `int main()` runs the graphical application. Unless stated otherwise, graphics-related construction and methods require a current OpenGL context with loaded GLAD functions and the expected named resources already loaded.

## Geometry: Ray, TriangleData, mollerTrumbore

[Header](../src/core/geometry.h) · [Implementation](../src/core/geometry.cpp)

```cpp
Ray(const glm::vec3& origin, const glm::vec3& direction);
TriangleData(unsigned int iA, unsigned int iB, unsigned int iC,
             const std::vector<Vertex>& vertices);
bool mollerTrumbore(const Ray& localRay,
                    const TriangleData& triangleData, float& tOut);
```

`Ray` stores the supplied origin and direction without normalization. `TriangleData` stores vertex indices, a reference to vertex A, edge vectors B-A/C-A, and a normalized cross-product normal. Indices must be valid, the triangle should be nondegenerate, and the vertex storage must outlive the triangle without reallocating its referenced position.

The ray and triangle must share coordinates, normally object coordinates. The function uses Moller-Trumbore intersection with `sceneData::intersectionEpsilon = 1e-6`: it rejects a near-zero determinant, tests barycentric coordinates with tolerance, and rejects `t < -epsilon`. Both faces can be hit. On success it writes `tOut`; on failure it leaves it unchanged. Very small negative hits can be accepted.

```cpp
#include "core/geometry.h"
#include <vector>

std::vector<Vertex> vertices{
    Vertex({0, 0, 0}, {0, 0, 1}, {0, 0}),
    Vertex({1, 0, 0}, {0, 0, 1}, {1, 0}),
    Vertex({0, 1, 0}, {0, 0, 1}, {0, 1})
};
TriangleData triangle(0, 1, 2, vertices);
Ray ray({0.25f, 0.25f, 1.0f}, {0.0f, 0.0f, -2.0f});
float t = -1.0f;
bool hit = mollerTrumbore(ray, triangle, t); // true; t = 0.5
```

The hit point is `ray.m_origin + t * ray.m_direction`. Here the origin-to-hit distance is 1, while `t` is 0.5. Do not label an unnormalized ray parameter as distance.

## Collision: BoundingBox and intersectBB

[Header](../src/core/collision.h) · [Implementation](../src/core/collision.cpp)

`BoundingBox(glm::vec2 pos, glm::vec2 size)` sets `m_min = pos`, `m_max = pos + size`, and `m_size = size`. `updateBB(pos)` relocates min/max using the stored size. Supply finite positions and positive sizes for ordinary rectangles; input validation is not implemented.

`bool intersectBB(const BoundingBox&, const BoundingBox&)` returns true only for strict overlap in X and Y. Edge/corner contact is false. It is a discrete test, not continuous collision detection.

```cpp
#include "core/collision.h"

BoundingBox a({0, 0}, {1, 1});
BoundingBox b({1, 0}, {1, 1});
bool touching = intersectBB(a, b); // false
b.updateBB({0.5f, 0.0f});
bool overlapping = intersectBB(a, b); // true
```

## Body movement: GameObj

[Header](../src/app/game2D/game_obj.h) · [Implementation](../src/app/game2D/game_obj.cpp)

`GameObj(position, size, velocity)` stores 2D vectors. It provides position/size/velocity getters and setters, including separate horizontal/vertical components. `updatePosition(float dt)` applies `position += velocity * dt`.

Use seconds for `dt`, game units for position/size, and game units per second for velocity. This method does not apply gravity, clamp world bounds, or update another object's bounding box. Player/cat logic does those operations separately.

```cpp
#include "app/game2D/game_obj.h"

GameObj body({0, 100}, {100, 100}, {200, 0});
body.updatePosition(0.5f); // position is (100, 100)
```

## Animation: Animator

[Header](../src/app/game2D/animator.h) · [Implementation](../src/app/game2D/animator.cpp)

| Method | Contract |
| --- | --- |
| `Animator(int sheetRows, int sheetColumns)` | Stores grid dimensions; constructor frame/row state starts at zero. Supply positive dimensions. |
| `setAnimation(int start, int end, int line, float fps)` | Sets the inclusive frame range, row, duration `1/fps`, current frame to start, and accumulator to zero. Use one-based frame/row indices within the sheet and positive finite FPS. |
| `updateUV(float dt, bool facingRight)` | Add elapsed seconds. If a frame duration is reached, advance one frame, wrap past end to start, and reset the accumulator. Return `(uLeft, vBottom, uRight, vTop)`; left-facing swaps U values. |
| `getFrame()` | Current frame index. |
| `isAnimationFinished()` | Whether current frame equals end; not a persistent completion event. |

Call `setAnimation()` before using a character animation. The code does not validate ranges/FPS and does not catch up multiple frames after a long update. A one-frame animation is already “finished” at its only frame.

```cpp
#include "app/game2D/animator.h"

Animator animation(6, 13);
animation.setAnimation(1, 9, 3, 10.0f);
glm::vec4 rightUV = animation.updateUV(0.0f, true);  // frame 1
glm::vec4 leftUV = animation.updateUV(0.0f, false);  // same frame, U swapped
animation.updateUV(0.5f, true); // advances only to frame 2
```

## Camera

[Header](../src/graphics/camera.h) · [Implementation](../src/graphics/camera.cpp)

`Camera()` uses `sceneData` defaults. `Camera(position, fovy, aspect, near, far)` initially faces the world origin. There is no target argument. Supply a nonzero position, a nondegenerate front direction, a positive aspect ratio, `0 < fovy < 180`, and `0 < near < far`; these conditions are caller responsibilities.

| Interface | Meaning |
| --- | --- |
| `getPerspectiveProjMatrix()` | Perspective matrix; constructor FOV is in **degrees** and converted to radians internally. |
| `getOrthoProj(width, height)` | Orthographic range `[0,width] x [0,height]` with near/far -1/1. |
| `getViewMatrix()`, `getTarget()` | View from position toward position + front; target getter returns that point. |
| `getPosition()`, `getFront()`, `getYaw()`, `getPitch()` | Position/direction and orientation; yaw/pitch are **radians**. |
| `setAspect()`, `setPosition()`, `setFront()` | Set projection aspect, position, or front. `setFront()` recomputes angles and basis; provide a normalized nonsingular front. |
| `setGrounded(float)`, `getGrounded()` | Float setter converts to the stored boolean. Forward/backward movement clamps Y to the stored altitude when grounded. |
| `moveForward/Backward/Right/Left/Up/Down(float dx)` | Displacement is keyboard sensitivity x basis direction x dx. These methods do not interpret dx as seconds. |
| `rotateOnXAxis()`, `rotateOnYAxis()` | Change yaw/pitch using mouse sensitivity; these helpers alone do not refresh basis vectors. |
| `processMouseInputs(offX, offY)` | Update angles/basis and clamp pitch to +/-89 degrees. Offsets are multiplied by sensitivity into radians. |
| `getMouseSpeed()`, `getKeyboardSpeed()` | Current source-defined sensitivities. |

In the application, `Scene` passes keyboard sensitivity as `dx`, so sensitivity is applied twice per movement call and no frame-time factor is used. Camera movement has no room collision.

```cpp
#include "graphics/camera.h"

Camera camera({2.0f, 1.7f, 0.0f}, 45.0f, 16.0f / 9.0f, 0.1f, 20.0f);
camera.processMouseInputs(10.0f, 0.0f);
glm::mat4 view = camera.getViewMatrix();
glm::mat4 projection = camera.getPerspectiveProjMatrix();
```

## Input, actions, callbacks, and utilities

| Interface | Contract |
| --- | --- |
| [InputManager](../src/core/input_manager.h) `registerKey`, `update`, `isPressed/Held/Released`, `isMouseButton` | Register a GLFW key/button, poll a valid window, then query its state. Initial registered state is Released; first down poll is Pressed, subsequent down polls are Held. Unknown keys query false. Mouse buttons use `glfwGetMouseButton`. |
| [ActionMap](../src/core/action_map.h) constructor, `bind`, `justStarted`, `ongoing`, `justFinished` | Holds a reference to an InputManager; bind replaces a logical action's key. Queries map to Pressed, Held, Released. Unbound actions return false. |
| [Callbacks](../src/core/callbacks.h) `initCallbacks` and callback functions | Register framebuffer/key/mouse callbacks; forward key/mouse events to ImGui. Escape requests window closure. Framebuffer callback updates only the viewport. |
| [Utilities](../src/core/utilities.h) `initGLFWwindow` overloads | Title/scale overload defaults to empty title and scale 1; sized overload takes width/height/title. Scale must be in [0,1] or it throws. GLFW/window failures return null; the existing caller does not fully guard all failures. |
| `initGLAD(window)` | Load GL entry points; false on failure, with window destruction/GLFW termination. |
| `getDeltaTime(float& lastFrame)` | Return current GLFW time minus lastFrame and update lastFrame, in seconds. |
| `printVec2/3/4`, `printMat4` | Print vectors or a matrix to stdout for diagnostics. |

`ActionMap::justFinished()` reports the **Released state**, so it remains true while a registered key is up; it is not a one-frame release event. `ongoing()` is false on the initial Pressed frame. InputManager and window/resource dependencies must outlive their users.

## Resources and graphics

| Interface | Methods/defaults and responsibilities |
| --- | --- |
| [ResourceManager](../src/core/resource_manager.h) | Static public shader/texture maps; `loadShader` accepts vertex/fragment paths or vertex/geometry/fragment paths plus a logical name; `loadTexture(path, TexParams&, name)` loads/flips a PNG and can fill zero dimensions. `getShader/getTexture` return references and throw on missing names. Texture-load failure logs and may return an ID-zero entry. |
| [Shader](../src/graphics/shader.h) | Default program ID 0; `linkProgram` overloads compile/link **source strings**; `use` returns a reference. `getID`, `setInt/Float/Vector2f/Vector3f/Vector4f/Matrix4fv` expose program/uniform operations. Activate the appropriate program before setting uniforms. Errors are logged; destructor deletes a nonzero program. |
| [TexParams / Texture](../src/graphics/texture.h) | TexParams defaults: dimensions 0, RGBA, repeat wrapping, linear filters. Texture defaults to ID 0; image constructor and `genColorMap` create color textures; `genDepthMap(width,height)` creates a 2D depth texture; `genCubeDepthMap()` uses scene shadow dimensions. `bind/unbind` operate on GL_TEXTURE_2D, so cube maps need explicit cube-map binding. `getID` returns the GL handle. |
| [Framebuffer](../src/graphics/framebuffer.h) | `Framebuffer(width,height,type)` supports `'c'` color, `'d'` 2D depth, `'m'` depth cube. `bind/unbind`, `getID/getTex/getWidth/getHeight` expose state. Cube allocation uses scene shadow dimensions; unknown type leaves ID 0. Completeness failure logs. |
| [Vertex](../src/graphics/vertex.h) | Position, normal, UV constructor/public fields and getters; no default constructor. |
| [Mesh](../src/graphics/mesh.h) | Public vertex/index/triangle collections and material index. `setup/draw/getVAO` manage upload/draw; `getCenter` averages vertex positions; `getGlobalNormal` normalizes summed normals, meaningful for planar meshes with a nonzero normal sum. `intersectRayTriangle` chooses the smallest accepted t. `resizeUV`, `m_mixTex`, and `getShader/setShader/useShader` control rendering. |
| [Model](../src/graphics/model.h) | `Model(filepath, Shader*)` imports OBJ and initializes meshes. `getMesh(name)` throws if absent; `getMaterial(name)` uses that **mesh's name** and material index. `draw`, `setShader`, `printMeshNames`, model-matrix access, and world-position methods operate on the model. `intersectRayTriangle` chooses the smallest accepted mesh hit. |
| [Material](../src/graphics/material.h) | Color/shininess setters, `setUseTex/toggleTex/overrideTex`, and `apply(Shader&)` set supported uniforms and optionally bind the referenced texture. Default colors/shininess are zero, texture null, texture use false. Texture override does not transfer ownership. |

Model/mesh intersections require an already transformed object-coordinate ray; they do not transform it internally. Empty hit searches return false without writing the output. Supply nonempty geometry for center/normal calculations. Imported OBJ files need normals; the loader does not generate them.

Keep shader/texture objects alive while pointers/references use them. Do not resize public model/mesh vectors after initialization: mesh lookup pointers and triangle vertex references can become invalid. The existing graphics wrappers are not complete RAII ownership types; see [lifetime limitations](architecture.md#resource-lifetime-and-error-handling).

## Scene and UI

| Interface | Methods and observable behavior |
| --- | --- |
| [Scene](../src/app/scene3D/scene.h) | Construct with initial framebuffer dimensions; `input3DHandler/cam3DinputHandler` process room input, `picking` tests the cabinet ray, `update` advances transitions, `drawScene/depthPass` draw lighting/shadows. Input getters/setters gate the two contexts. Camera/animation getters, `cameraInAnimation/cameraOutAnimation`, and `switchCameraGrounded` control camera state. Display methods wrap Arcade; aim and menu/help methods expose flags. |
| [Arcade](../src/app/scene3D/arcade.h) | Construct from model path; `setScreen` references a texture; `screenSwitch/isDisplayOn` control power; `setShader` chooses CRT for a powered display during the normal lighting pass. `draw/setWCSPosition/getModel` wrap the model. |
| [Room](../src/app/scene3D/room.h) | Construct from model path and optional floor texture pointer (default null). `setFloor` overrides/tile-enables the floor material. `setShader/draw/setWCSPosition/getModel` wrap the model. |
| [UIManager](../src/app/ui_manager.h) | `init(window)`, `beginFrame()`, `drawUI(scene,game,width,height)`, `render()`, `shutdown()`; `isMenuOpen()` reports menu state from the latest UI draw. Init/shutdown guard repeated calls. UI draw changes input and scene/game state through menu buttons. |

Scene animation methods return whether the relevant animation remains active; they are stepped over frames, not blocking operations. Cabinet selection has no distance threshold or room-occlusion check. Screen texture, display mesh/material names, floor names, and lamp names must match the supplied assets. The [tutorial](tutorial.md) is the working user-level example of these interfaces.

## Game and sprite interfaces

| Interface | Methods and responsibilities |
| --- | --- |
| [Game](../src/app/game2D/game.h) | Construct with target dimensions; `input2DHandler` routes player and automatic cat input; `update(dt)` moves/animates, manages projectiles/collisions and death menu; `render()` draws to its FBO. `resetGame()` invokes component resets. Getters expose sprite renderer, FBO texture, camera X, cat lives, and menu/help state; methods close the menu/toggle help. |
| [Player](../src/app/game2D/player.h) | Texture-name/position/size/velocity constructor; `inputHandler/update/render/resetPlayer`; getters expose body, bounds, position, size, texture, state. `hasBeenHit()` sets a flag consumed by update. Attacks fire at configured animation frame 5. |
| [Cat](../src/app/game2D/cat.h) | Same basic construction data; `autoInput/update/attackHandler/jumpMoveHandler/render/resetCat`; getters expose body, bounds, position, state, lives. `hasBeenHit()` reduces lives while positive and enables the barrage flag. Automatic attacks use timing, player position, and life-count triggers. |
| [Projectile](../src/app/game2D/projectile.h) | Texture-name/position/size/velocity/facing/owner constructor. `update/render`; body/position/size/velocity/owner/status getters; `setStatus(false)` marks inactive. Owner is Player or Cat. |
| [ProjectileManager](../src/app/game2D/projectile_manager.h) | `emit` owns a new projectile, `update(dt,right,left,top,bottom)` moves and removes inactive/out-of-bounds projectiles, `render` draws, `resetProjSys` clears, `getProjectiles` returns the mutable owned vector. Boundary checks use projectile position, not its full rectangle. |
| [CollisionSystem](../src/app/game2D/collision_system.h) | Construct with player/cat/projectile-manager references. `checkCollision()` tests cat projectiles against player bounds and player projectiles against cat bounds, marks projectiles inactive, and notifies targets. No general world/body collision. |
| [Sprite](../src/app/game2D/sprite.h) | Texture name plus optional sheet rows/columns (both default 1). `setAnimation/updateTexture/render`; texture/animator getters. References an already-loaded texture. |
| [SpriteRenderer](../src/app/game2D/sprite_renderer.h) | Construct with projection dimensions; `drawSprite(shader,texture,position,size,uv)` draws a quad; default UV is (0,0,1,1). |
| [Background](../src/app/game2D/background.h) | Texture-name/position/size constructor; `render` accepts optional position/size; each (0,0) argument selects the stored default, so an explicit zero cannot override a nonzero stored value. |
| [GameHUD](../src/app/game2D/gameHUD.h) | Life-texture-name constructor; `render(renderer,shader,position,size)` draws a life icon. |
| [State](../src/app/game2D/state.h) / AnimationData | States Idle/Walk/Jump/Attack/Hit/Dead; `stateToString` returns uppercase names or UNKNOWN. AnimationData stores start/end/row/FPS. |

All gameplay `dt` values are seconds. Names must match entries loaded by `assetLoader()`; these constructors are not independent of graphics resources. Reset methods restore selected fields rather than reconstructing every object; bounds are normally refreshed on update. Collision marks inactivity after the manager update, so removal usually occurs on the next update.

The source interfaces are broader than the currently automated coverage. Use the [testing record](testing.md) for actual tested cases and outstanding gaps; the tables above do not imply that every method has a passing unit test.
