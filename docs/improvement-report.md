# Improvement report

> Historical audit of `sac4ap`. Proposals were subsequently authorized on **sac4ap-dev**; see [implementation report](improvements-implemented.md). Descriptions below refer to the earlier code.

[Documentation index](index.md) · [Testing scope](testing.md) · [Exam readiness](exam-readiness.md)

This report records proposed production changes for the owner's review. **None
of the fixes below has been applied in `sac4ap`.** Tests/documentation describe the
current code instead. Priorities reflect user impact, not a claim that every
item has already caused a failure in the normal demonstration.

## Priority 1: reliability and reproducibility

### Fail startup clearly when initialization or resources are missing

In [main.cpp](../src/main.cpp), `main()` uses the result of `initGLFWwindow()`
before checking it. The initializer can return null. In
[resource_manager.cpp](../src/core/resource_manager.cpp), failed file reads return
an empty string and failed texture reads still return the map's default texture;
shader/model errors may log and continue. This can turn a missing driver or asset
into a later failure with an unclear cause.

Proposed change: stop initialization with an informative error and a nonzero
exit status, propagate resource failures, and keep context lifetime explicit.
Before fixing, add tests using a failed window initializer, a missing texture,
a missing model and an invalid shader. The asset preflight catches the current
checkout's file issues but does not replace runtime error handling.

### Pin and verify dependency contents

The unchanged [CMakeLists.txt](../CMakeLists.txt) retrieves `stb` from `master`
and the Assimp archive without `URL_HASH`. A future configure can therefore use
different dependency contents even at the same RoyalGreen commit. The present
application also depends on older GLM CMake policies and MSVC extensions.

Proposed change: select immutable commit hashes, add archive checksums and
explicitly test dependency/compiler upgrades. Keep a known-good dependency
manifest. This is a build-system change and has been deferred under the
no-internal-code-change constraint. Current versions and the recorded `stb`
commit are listed in [dependencies](dependencies.md) and [validation](validation.md).

### Package assets independently of the original checkout path

CMake embeds the configuring checkout's absolute resource directories into the
executable through `SHADERS_DIR`, `MESHES_DIR` and `TEXTURE_DIR`. A fresh build
works in another checkout, but copying only the resulting executable or moving
the checkout after building can break loading. `install()` copies only the
executable. The original released binary's `C:\dev\RoyalGreen` instructions do
not make new builds portable packages.

Proposed change: install/copy resources with the binary and resolve them relative
to the executable or an explicit runtime asset-directory option. Verify from a
clean installation folder and an unrelated working directory. The current
supported workflow is building from a retained checkout.

## Priority 2: stable timing and precise API contracts

### Make camera movement independent of frame rate

[Scene::cam3DinputHandler](../src/app/scene3D/scene.cpp) passes
`getKeyboardSpeed()` to camera movement methods, which multiply by that speed
again. No elapsed time enters this path. With the default speed `0.15`, the
translation is `0.0225` scene units per held frame: approximately `1.35` units/s
at 60 FPS and `2.7` units/s at 120 FPS. In contrast, the 2D body uses velocity
times `dt`.

Proposed change: pass elapsed seconds once and define speed in scene units/s.
Use tests that compare one second split across 30, 60 and 120 updates. Keep the
current movement tests until the API change is accepted, then update its contract
and expected results together.

### Preserve animation elapsed remainder and handle large time steps

[Animator::updateUV](../src/app/game2D/animator.cpp) advances at most one frame
per call and resets accumulated time to zero. At low frame rates or after a
stall, the animation runs slower than its configured FPS. The suite has a
regression case that makes this behavior explicit.

Proposed change: consume complete frame intervals and retain the remainder.
Define whether shoot/end-frame events are delivered for skipped frames before
changing this: `Player` and `Cat` rely on observing particular frame numbers.
Test both time partition invariance and delivery of gameplay events.

### Distinguish a release event from a key that is up

[InputManager](../src/core/input_manager.cpp) retains `Released` indefinitely,
including immediately after registration. Consequently
[ActionMap::justFinished](../src/core/action_map.cpp) returns true repeatedly
while a bound key is up. Its name suggests a one-frame event.

Proposed change: either rename/document it as a level query, or store a separate
previous state and expose a release edge. Specify compatibility before changing
callers. Test initial registration, press/hold/release and subsequent up frames.

### Define invalid geometry, camera and animation input behavior

Triangle indices/lifetimes, non-degenerate geometry, finite nonzero camera/front
vectors, positive aspect and clipping distances, positive sheet sizes and FPS
are caller preconditions today. Several setters/constructors accept values that
would yield invalid indexing, division or non-finite vectors. `TriangleData`
borrows a vertex but caches edges, so later vertex movement also invalidates the
geometric contract.

Proposed change: decide which API boundaries validate inputs, report meaningful
errors, and consider owned/precomputed triangle data if mesh mutation becomes a
feature. Add focused negative tests **before** changing production behavior.
The current tests deliberately do not invoke undefined behavior.

## Priority 3: integration testing and maintainability

### Test gameplay and lifecycle boundaries independently of rendering

The current suite executes CPU character/projectile/collision behavior with default
ID-zero textures in the real resource map, and tests CPU mesh calculations.
Full scene/game coordination, file loading, GPU lifecycle and menu interactions
are still outside those paths.

Earlier tightly coupled paths in `Player`, `Cat`, `CollisionSystem`,
`ProjectileManager` show that some logic can already be exercised independently of rendering.
The remaining coordinators/import/menu paths contain processing as well as visualization;
a manual checklist and startup smoke test cover only part of that gap.

The component tests now reproduce these existing behaviors:

- A player attack requested immediately after the final attack frame can end
  before firing; an intervening idle animation update permits the next attack.
- Updating a Dead cat directly returns it to Hit. The normal `Game::update()`
  stops updating a Dead actor and therefore masks this transition.
- Calling collision checking again before manager cleanup damages targets again
  even for already inactive projectiles. The normal game loop removes inactive
  projectiles before its next collision pass.
- Player/cat reset methods leave the bounding box at its previous location until
  the next update. Bounds can also lag a ground snap performed after their update.

These are passing **characterization** tests of legacy behavior, not claims that
such behavior is desirable. Tests are linked in [testing](testing.md).

Proposed change: isolate resource/window dependencies and define update order,
terminal states, collision idempotence and full reset contracts. Extend the
component tests to the full game/menu sequence, including movement after closing
menus. Review `resetCat()` flags not explicitly restored from its constructor.
Write failing tests for the newly agreed behavior before implementing approved
fixes; keep the present characterization cases until those decisions are made.

### Give GPU objects explicit ownership and destruction order

Textures, mesh buffers and framebuffers do not visibly expose a complete cleanup
policy; shaders in the static resource maps have destructors while `main()`
terminates the context before those maps are destroyed. The observed startup/
shutdown smoke test succeeds; that does not measure GPU lifetime correctness.

Proposed change: document/implement ownership, prevent accidental resource
copies, release GPU objects before terminating GLFW, and test repeated scene/game
creation and shutdown under a real context. Measure resource use before claiming
leaks or improvements.

### Add configurable runs and decide distribution licensing

The normal demo works through the current controls and menus. Its constants are
compiled in; no command-line options or external configuration are supported.
A future parameter file could expose resolution, asset location and timing
without source edits. Review the existing Windows-specific GPU export before
promising other operating systems.

The project currently has no root project license. Choosing one and recording
asset provenance would clarify reuse/distribution terms. No license has been
selected on the owner's behalf, and no existing third-party notices were removed.

## Suggested acceptance order

1. Agree the intended platform and demonstration with the examiner.
2. Review startup errors, dependency pinning and packaging first.
3. Decide timing and input API semantics; write failing regression tests for the
   newly intended behavior before implementing the accepted fixes.
4. Separate gameplay logic enough to close the integration testing gap.
5. Measure coverage/performance only over a clearly stated scope.

Each accepted change should receive its own descriptive commit, updated API/user
documentation and passing tests. Preserve the original history; there is no
reason to amend old commits or backdate resumed work.