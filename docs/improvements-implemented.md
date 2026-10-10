# Approved improvements in sac4ap-dev

[Documentation index](index.md) · [Original audit](improvement-report.md) · [Validation](development-validation.md)

This branch descends from completed `sac4ap`. Production changes were subsequently authorized here. Original/exam branches and all their historical commits remain; runtime assets/GLAD contents are unchanged.

| Proposal / added request | Implementation | Verification |
| --- | --- | --- |
| Clear startup failures | Validate window/GLAD; exceptions for missing/invalid resources, nonzero startup status | Fake window failures, missing-file cases, actual context error checks |
| Immutable dependencies | Git hashes, archive SHA256, header-only GLM, portable C++ calls/standard member offsets | Native clean CI builds |
| Relocatable package | Executable-relative/explicit asset root, copy/install, static Windows runtime, ZIP/notices | PE/asset bytes; relocated package launched from unrelated directory |
| Camera timing | Pass elapsed seconds once; speed 1.35 units/s, immediate basis refresh | Equal travel at 30/60/120 updates |
| Animation timing/events | Full intervals/remainder, crossed-frame events, nonlooping combat | Partition, skipped events, consecutive attacks |
| Release edge | Up distinct from one-sample Released | Registration/press/hold/release/up |
| Valid inputs / geometry ownership | Finite/range checks, owned triangle snapshot; skip zero-area imported faces | Negative/boundary/lifetime tests, original model import |
| Gameplay/reset integration | CPU GameSession, coherent bounds, terminal deaths, reset flags, inactive shots ignored | Pause/resume/reset/rematch and component regressions |
| GPU ownership | Move-only owners, allocation destructors, context outlives resources | Copy rejection and real handle deletion |
| Configurable sessions | CLI/config resolution/camera/assets, CLI precedence | Parsing/error/path cases, packaged smoke |
| Fixed initial screen | Offset always zero; actors clamped inside 600 x 600 arena | Both margins, bounds and fixed camera |
| Resize/high DPI | Physical sizes for projection, logical sizes for mouse/UI | Build coverage; interactive scaling/focus remains manual |
| Distribution licensing | Inventory and upstream notices | Owner's license/asset provenance decisions pending |

The [manual checklist](manual-testing.md) covers appearance, menus, focus and repeated sessions. Whole-project memory/coverage measurements and exhaustive driver/hardware gameplay checks remain outside completed evidence.
