# API reference

[Documentation index](index.md) · [Architecture](architecture.md) · [Testing scope](testing.md)

RoyalGreen is an executable, not an installed SDK. Source examples use `src/` include paths and require the corresponding implementation files/dependencies. Graphics allocation/drawing/deletion requires a current context and loaded GLAD functions. Default ID-zero owners permit CPU fixtures without GPU calls.

## Runtime options and assets

[Header](../src/core/runtime_options.h) · [Paths](../src/app/path.h)

`runtime::parseOptions(const std::vector<std::string>&)` returns `Options`. Defaults: dimensions 0/0, monitor scale 0.7, camera speed 1.35 units/s, sensitivity 0.001 radians/cursor unit, no automatic close. It recognizes flags/config keys listed in the [tutorial](tutorial.md). Invalid values/unknown names throw `invalid_argument`; unreadable configuration throws `runtime_error`. CLI overrides config. Relative config assets resolve beside the config file.

`executablePath()` returns the real executable location using the platform API. `resolveAssets(executable, explicitRoot={})` validates shaders/meshes/textures directories. Explicit invalid roots throw; default search checks `res/` beside the executable, one directory above, then `../share/royalgreen/res`. `Path::configure(root)` assigns the named resource paths before loading. No source-checkout path is compiled into the EXE.

## Geometry and collision

[Geometry header](../src/core/geometry.h) · [Collision header](../src/core/collision.h)

```cpp
Ray(const glm::vec3& origin, const glm::vec3& direction);
TriangleData(unsigned int a, unsigned int b, unsigned int c,
             const std::vector<Vertex>& vertices);
bool mollerTrumbore(const Ray& ray, const TriangleData& triangle, float& tOut);
BoundingBox(glm::vec2 position, glm::vec2 size);
bool intersectBB(const BoundingBox& a, const BoundingBox& b);
```

Ray requires finite origin/nonzero finite direction and keeps its magnitude. TriangleData owns vertex-A/edge/normal snapshots; invalid indices throw `out_of_range`, nonfinite/zero-area geometry throws `invalid_argument`. Editing/reallocating/destroying vertex storage cannot invalidate a snapshot; rebuild it to reflect edits.

Ray/triangle coordinates must match, normally object coordinates. Moller-Trumbore is two-sided; epsilon 1e-6 applies to determinant, barycentric bounds and negative t rejection. Tiny negative t within tolerance is accepted. A miss leaves `tOut` unchanged; a hit writes the parameter in `origin+t*direction`, a distance only for unit directions.

```cpp
std::vector<Vertex> vertices{
    Vertex({0,0,0}, {0,0,1}, {0,0}),
    Vertex({1,0,0}, {0,0,1}, {1,0}),
    Vertex({0,1,0}, {0,0,1}, {0,1})
};
TriangleData triangle(0,1,2,vertices);
float t=-1;
bool hit=mollerTrumbore(Ray({0.25f,0.25f,1},{0,0,-2}),triangle,t);
// hit=true, t=0.5; hit distance is 1.
```

BoundingBox requires finite position, positive finite extents and finite maximum corner. It stores lower-left min, size and max=min+size; `updateBB(position)` relocates both corners and validates finiteness. Strict X/Y overlap is required: edge/corner contact is false. Public fields must remain valid if edited directly; intersection is discrete with no epsilon/swept collision.

## Input and utilities

[InputManager](../src/core/input_manager.h) · [ActionMap](../src/core/action_map.h) · [Utilities](../src/core/utilities.h)

Register keyboard/mouse codes separately from action binding. Poll GLFW events, call `InputManager::update(window)` once, then query. Registration starts Up. Down samples transition Up/Released -> Pressed -> Held; up samples transition Pressed/Held -> Released -> Up. Pressed/Released last one sample. Unknown codes query false. Polling may miss taps between samples.

ActionMap borrows InputManager, which must outlive it. `bind` replaces an action's code; `justStarted`, `ongoing`, `justFinished` query Pressed, Held, Released. Held excludes the first Pressed sample; unbound/unregistered inputs return false.

`initGLFWwindow(title,scale)` checks finite scale in (0,1]; the sized overload checks positive dimensions. GLFW/window failures return null. `initGLAD(window)` returns false for null/load failure without destroying caller-owned resources. Main owns window/GLFW cleanup. `getDeltaTime(float& last)` returns seconds and advances last; main initializes last after startup and caps long frames.

Callbacks forward key/mouse events to ImGui and Escape requests close. Framebuffer callback changes the viewport; main separately refreshes scene projection and logical UI dimensions.

## Camera

[Header](../src/graphics/camera.h)

`Camera(position,fovy,aspect,near,far)` faces the origin. Position must be finite, nonzero and not parallel to world Y, 0<fovy<180 degrees, aspect>0 and 0<near<far. Invalid inputs throw `invalid_argument`. Default construction uses scene defaults with horizontal -X front.

| Interface | Contract |
| --- | --- |
| `moveForward/Backward/Right/Left/Up/Down(float dt)` | Finite signed seconds, displacement speed*basis*dt; reverse sign reverses direction |
| `setKeyboardSpeed`, `setMouseSpeed` | Finite nonnegative scene units/s or radians/cursor unit |
| `setGrounded(float)` | Nonzero becomes true; captures current Y; forward/backward then lock that altitude |
| `setPosition(vec3)` | Finite position and saved height, orientation retained |
| `setFront(vec3)` | Finite nonzero direction, normalize, clamp pitch +/-89 degrees and refresh basis |
| `rotateOnXAxis/YAxis`, `processMouseInputs` | Finite offsets*sensitivity, immediate basis refresh; pitch clamped |
| `setAspect` | Finite positive ratio |
| `getViewMatrix`, `getTarget` | View/target from eye toward eye+front |
| `getPerspectiveProjMatrix` | FOV degrees converted to radians |
| `getOrthoProj(width,height)` | Positive dimensions, [0,width] x [0,height], near/far=-1/1 |
| `getPosition/Front/Yaw/Pitch/MouseSpeed/KeyboardSpeed/Grounded` | Position, unit direction, angles in radians, current settings |

Right/left use local right. Up/down use local up and update saved altitude when grounded. No room collision is implemented.

```cpp
Camera camera({2,1.7f,0},45,16.0f/9,0.1f,20);
camera.setKeyboardSpeed(1.35f);
camera.moveForward(1.0f/60); // one sixtieth of a second
camera.processMouseInputs(10,0);
```

## Body, animation and sprites

[GameObj](../src/app/game2D/game_obj.h) stores position/size/velocity; `updatePosition(dt)` applies position+=velocity*dt. It alone does not apply gravity, enforce arena bounds or refresh another object's bounding box. Positions/sizes are game units, velocity units/s and elapsed time seconds.

[Animator](../src/app/game2D/animator.h) requires positive sheet rows/columns and starts with a valid one-cell clip. `setAnimation(start,end,row,fps,bool loop=true)` validates one-based ranges/row and positive finite FPS, resets current frame/remainder. `updateUV(dt,facingRight)` validates finite nonnegative time, consumes all complete intervals and retains remainder. Returned UV is (uLeft,vBottom,uRight,vTop); left facing swaps U. Nonlooping clips clamp at end; looping clips wrap.

`isAnimationFinished()` means current==end, persistent only for nonlooping clips. `passedFrame(frame)` includes the current frame and all crossed frames in the last update. A one-cell clip is already finished. Gameplay uses a separate emitted-shot guard, so a shoot frame crossed after a long update still fires only once.

```cpp
Animator clip(6,13);
clip.setAnimation(1,8,2,25,false);
clip.updateUV(0.5f,true);
// final frame 8; passedFrame(5)=true and isAnimationFinished()=true
```

[Sprite](../src/app/game2D/sprite.h) borrows a named Texture; setAnimation forwards loop mode, updateTexture applies UV, render draws through SpriteRenderer. Names must exist before construction. SpriteRenderer owns its quad VAO/VBO and is noncopyable. Background's (0,0) optional render position/size means use its stored default; GameHUD draws life icons.

## Resources and OpenGL owners

| Interface | Contract |
| --- | --- |
| [ResourceManager](../src/core/resource_manager.h) | Named public maps own shaders/textures. loadShader accepts stage file paths/name, reads before compilation, then inserts only on success. loadTexture(path,TexParams **by value**,name) decodes/flips PNG/JPEG to RGBA, fills zero dimensions, rejects explicit mismatch and inserts only on success. getShader/getTexture throw runtime_error for unknown names. clear must run with context alive after borrowers are gone. |
| [Shader](../src/graphics/shader.h) | Default ID0; move-only program owner. linkProgram accepts stage **source strings**, throws with compiler/linker log and preserves previous program on failure. use throws for ID0. Uniform setters require the appropriate program active. Destructor deletes nonzero program; partial stage/program cleanup is exception-safe. |
| [Texture / TexParams](../src/graphics/texture.h) | ID0 default; move-only. Params default dimensions0, RGBA, repeat, linear. Color/depth allocations need positive sizes and an unallocated owner. Cube depth uses scene shadow dimensions. bind/unbind use its stored 2D/cube target. Destructor deletes its handle. |
| [Framebuffer](../src/graphics/framebuffer.h) | Positive dimensions, type c/d/m for color/depth/cube; invalid type throws, incomplete FBO throws. Owns its texture and FBO; noncopyable. bind/unbind and ID/texture/dimension getters expose it. Cube faces use scene shadow dimensions. |
| [Mesh](../src/graphics/mesh.h) | Move-only VAO/VBO/EBO owner with public vertices/indices/material index. setup uploads, draw issues GL draw. getCenter requires nonempty geometry, getGlobalNormal a finite nonzero normal sum. rebuildTriangleCache snapshots indexed geometry, omits zero-area faces without altering draw indices; bad index/range fails. intersectRayTriangle writes nearest accepted t on hit only. Shader pointer is borrowed; useShader throws if unset. |
| [Model](../src/graphics/model.h) | Imports OBJ/MTL, initializes meshes and requires a nonnull borrowed shader. Missing/bad import throws. getMesh throws for missing name; getMaterial(string) identifies the material via a **mesh name**. draw/setShader/world/model matrix methods operate on model. Picking requires object-coordinate ray and returns nearest t. Keep mesh vector storage stable after named lookup creation. |
| [Material](../src/graphics/material.h) | Colors/shininess/useTexture uniforms and optional borrowed texture; defaults zero colors/shininess, null texture, use=false. overrideTex transfers no ownership. |

GPU objects must be released before context termination. Do not clear/replace map entries while sprite/material references are live. Public geometry changes require cache rebuilding/upload and stable model lookup storage. Imported normals/material names remain asset contracts.

## Gameplay coordination

[GameSession](../src/app/game2D/game_session.h) constructs a CPU encounter from arena width/height; dimensions must contain actors and ground. Named actor/projectile textures are prerequisites even without rendering.

| Interface | Meaning |
| --- | --- |
| `input(actions,dt)` | Menu/help then player commands/cat AI; paused menu blocks actor input |
| `update(dt)` | Finite nonnegative seconds; paused sessions stop; actors/bounds/projectiles/collisions update in order; death opens menu |
| `reset()` | Clear shots, initial positions/velocities/flags/timers/clips/lives/bounds immediately; menu closed, help preference retained |
| `cameraX()` | Always zero |
| `isMenuOpen/closeMenu/help/toggleHelp` | CPU menu/help state |
| `player/cat/projectiles` | Owned components exposed for coordination/fixtures |

[Game](../src/app/game2D/game.h) wraps this session in background/HUD/quad rendering and a framebuffer. Its existing input/update/reset/menu methods forward to the session; render draws a fixed view. The game texture is borrowed by the cabinet until the game is destroyed.

Player and Cat own body/bounds/state and borrow sprites. update handles gravity/clips and state transitions; attacks use crossed shoot frames; reset refreshes bounds immediately. Dead is terminal until reset. constrainToArena(width) clamps the full quad to [0,width-size.x] and refreshes bounds. Player hit starts a terminal hit/death sequence; cat hits reduce its nine lives and enable its barrage condition.

ProjectileManager owns emitted projectiles, moves and removes inactive/out-of-bounds shots, renders and resets. Boundary cleanup tests projectile position, not its full quad. Inactive Projectile::update does nothing. CollisionSystem borrows actors/manager, ignores inactive shots and marks an accepted hit inactive immediately, preventing duplicate hits before removal.

## Scene and UI

[Scene](../src/app/scene3D/scene.h) coordinates room/cabinet/lights and state. input3DHandler/cam3DinputHandler take elapsed seconds; resize refreshes dimensions/aspect. Picking tests cabinet geometry without a selection-distance/room-occlusion limit. Camera approach/return are stepped transitions; mutually exclusive input flags gate exploration/game commands. Static shadow maps are built by depthPass; drawScene renders room/aim. Its owned GL objects are destroyed with context alive.

Arcade borrows game screen texture and selects CRT/power-button appearance. Room wraps its model/tiled-floor texture. Asset mesh/material/shader logical names must match.

[UIManager](../src/app/ui_manager.h) initializes checked ImGui backends, begins/draws/renders frames and shuts them down through destructor/explicit shutdown. drawUI receives logical window dimensions; GPU viewport uses physical framebuffer dimensions. Menu buttons change scene/game state. It is noncopyable and init/shutdown guard repeated calls. See the tutorial for menu labels and manual checklist for full UI behavior.

The reference is broader than automated coverage; [testing](testing.md) states what is actually asserted.
