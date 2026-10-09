# Manual testing

[Documentation index](index.md) · [Automated testing](testing.md) · [Tutorial](tutorial.md)

This checklist specifies expected outcomes for the interactive Windows application. **All result fields below are blank: this document does not claim these checks were run.** Record Pass, Fail, or Blocked, with a dated observation and evidence. Automated tests and successful compilation do not substitute for these observations.

## Session record

| Field | Value to fill in |
| --- | --- |
| Tester and date/time | |
| Git branch and full commit SHA | |
| Windows version and architecture | |
| Visual Studio / MSVC / Windows SDK | |
| CMake version | |
| Build configuration (Debug/Release) | |
| GPU, driver, and OpenGL renderer string | |
| Monitor dimensions / scaling | |
| Clone path and executable path | |
| Automated-check log references | |
| Screenshots or recording location | |

Use the build commands from [installation](installation.md). Start at the initial window size. Complete both configurations where practical, and record which configuration each observation concerns. The console prints a renderer string; preserve asset/shader/framebuffer diagnostics. Avoid changing tracked resources to manufacture failures in the working checkout.

## Startup, exploration, and selection

| ID | Procedure | Expected outcome | Result / evidence |
| --- | --- | --- | --- |
| M01 | Launch a locally built app with the original clone/resources present. | A room window appears with models, floor texture, aiming cross, helper, and FPS display; cabinet screen starts off. No asset/shader/FBO failure diagnostics. | |
| M02 | Hold W/A/S/D individually and move the mouse. | Camera moves/look direction changes; inputs affect the room. Observe frame-rate-dependent movement as an existing limitation. | |
| M03 | Look upward/downward, move forward, toggle F, and repeat. | With fixed height off, forward/backward can change height; with it on, forward/backward preserve the stored altitude. | |
| M04 | Press H twice, then T twice. | Each room overlay toggles and returns to its initial visibility; FPS overlay remains. | |
| M05 | Aim away from the cabinet and left-click. | Cabinet menu does not open. | |
| M06 | Aim at the cabinet and left-click. | Arcade menu opens and the cursor becomes usable for menu buttons; room movement is gated while the menu is open. | |
| M07 | Choose Exit in the Arcade menu. | Menu closes, screen remains off, mouse capture and exploration resume. | |
| M08 | Select the cabinet again and choose Play; try movement during the approach. | Screen powers on, button emission changes, camera approaches in two stages, ordinary camera movement is blocked during the approach, then game input is enabled. | |

## Gameplay, menus, and reset

| ID | Procedure | Expected outcome | Result / evidence |
| --- | --- | --- | --- |
| M09 | Hold A and D separately after approach completes. | Player walks left/right and faces the movement direction. Camera stays focused on the cabinet rather than moving around the room. | |
| M10 | Press W, attempt another jump while airborne, and wait to land. | Jump follows gravity and returns to the ground; a second jump is not started while already jumping. | |
| M11 | Face each direction and press S while grounded. | One arrow is emitted during each attack, travels in that direction, and movement resumes after the attack animation. | |
| M12 | Try shooting while airborne and holding S through an attack. | Airborne attack is not started; holding S is not a documented continuous-fire control. Note any state/timing anomaly. | |
| M13 | Move right far enough to scroll; return left. | Background/player view follows the horizontal camera, its position remains nonnegative, and life icons remain in the intended HUD region. | |
| M14 | Observe the cat without attacking, then approach or reduce its lives. | Cat faces/reacts to the player, emits energy balls, and can perform its jumping barrage. Record which trigger was observed. | |
| M15 | Hit the cat with arrows. | Each accepted projectile hit reduces remaining cat lives; life indicators track the count. An arrow disappears after collision/cleanup. | |
| M16 | Let a cat projectile hit the player. | Player enters the hit/death sequence; a later game update opens Game Menu. Record any delay or failure rather than assuming death handling works. | |
| M17 | Reset and defeat the cat by reducing its lives to zero. | Cat reaches the terminal sequence and Game Menu opens when death is observed; no separate victory screen is promised. | |
| M18 | Press H twice during active gameplay. | The 2D helper toggles independently of the room helper. | |
| M19 | Open M during motion or an attack, wait, then choose Return to game. | Menu offers all three buttons, gameplay update pauses while it is open, and choosing Return resumes the encounter. | |
| M20 | Fire projectiles, alter cat lives, move/scroll, then choose Reset game. | Player starts at (0,100), cat at (500,100), nine lives return, prior projectiles clear, and scrolling restarts. Check for stale combat/reset flags over repeated resets. | |
| M21 | Choose Exit game. | Game resets, screen turns off, camera moves away from the cabinet, then exploration resumes with grounded movement. | |
| M22 | Repeat Play, Return, Reset, Exit game several times; click/press controls around transitions. | No stuck menu/input state, unexpected movement during approach, or crash. Record transition defects with a minimal sequence. | |
| M23 | Let projectiles leave the game view. | They are removed instead of accumulating or reappearing on return. Visual observation does not prove memory/resource correctness. | |

## Graphics, environment, and termination

| ID | Procedure | Expected outcome | Result / evidence |
| --- | --- | --- | --- |
| M24 | Inspect room, floor tiling, cabinet, pool table, lamps, and shadows from several angles. | Models/materials are present, floor repeats, and static point-light shadows are visible without obvious missing assets. Record visual artifacts. | |
| M25 | Inspect game sprites, left/right facing, transparency, life icons, and CRT screen. | Correct sprite cells and facing, visible game on the screen, no missing texture/transparent-quad artifacts. | |
| M26 | Resize and minimize/restore the window. | Document the actual result. Full resize handling is not guaranteed: viewport changes without consistently updating scene/game/UI dimensions. | |
| M27 | Switch focus away and back during exploration, a menu, and gameplay. | Record cursor/input behavior and whether the session remains usable; no complete focus-handling guarantee is claimed. | |
| M28 | Close with Esc in separate room/menu/game sessions; also test the window close button. | Process exits without a visible crash or hang. This does not establish correct GL resource destruction. | |
| M29 | Launch the executable by full path from another working directory, keeping its configured clone in place. | Assets still load from the embedded clone paths. | |
| M30 | Clone sac4ap and build on a second Windows machine, then repeat M01, M08-M11, M19-M21, and M28. | App builds/runs with that machine's local resources; record its tool/GPU versions and exact results. | |

## Failure-path observations and limitations

If testing missing/damaged assets, use a separate disposable clone/build configured to that clone, not the tracked working resources. The static verifier should reject a missing/empty referenced asset or damaged PNG. The application itself has mixed failure handling and is not guaranteed to recover cleanly. Capture diagnostics and process behavior; do not list graceful recovery as a required existing feature.

Tests for source-level algorithms and APIs, including tolerance boundaries, strict edge contact, large animation steps, invalid-input preconditions, projectile removal, and reset-state details, belong in the [automated coverage discussion](testing.md). Manual observation cannot prove their complete numerical coverage.

For each failure, record the ID, starting state, exact inputs, expected/observed behavior, build/commit, and evidence. Put completed findings in the [validation record](validation.md). An unperformed second-machine check or unexecuted scenario must remain explicitly unverified.
