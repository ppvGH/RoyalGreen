# RoyalGreen documentation

[Repository home](../README.md)

These English guides describe **sac4ap-dev**, the continuation of the original C++17/OpenGL 3.3 arcade room and embedded 2D game. Documentation for the behavior-preserving adaptation remains on the `sac4ap` branch.

| Need | Guide |
| --- | --- |
| Run the Windows package or build on Windows/Linux/macOS | [Installation](installation.md) |
| Play and configure a session | [Tutorial](tutorial.md) |
| Understand coordination, state, rendering and ownership | [Architecture](architecture.md) |
| Look up signatures, units, errors and examples | [API](api.md) |
| Run tests and understand their scope | [Testing](testing.md) |
| Check the interactive demonstration | [Manual testing](manual-testing.md) |
| Identify immutable library revisions and notices | [Dependencies](dependencies.md) |
| Maintain protected branches and review new changes | [Contributing](contributing.md) |
| See how the original proposals were implemented | [Implementation report](improvements-implemented.md) |
| Inspect actual local/CI evidence | [Development validation](development-validation.md) |
| Relate the project to course requirements | [Exam readiness](exam-readiness.md) |

The [original audit](improvement-report.md) and [original validation](validation.md) are historical records for `sac4ap`. Current commands, contracts and results are recorded separately.

The application exposes exploration, cabinet selection, an autonomous enemy, fixed arena movement, jumping, attacks, pause/resume/reset and return to exploration. Runtime flags/configuration cover resolution, camera settings and asset location. No source edits are needed for an ordinary demonstration.

Tests assert numerical and behavioral outcomes. Manual checklists give expected outcomes with blank result fields. Screenshots illustrate appearance. A configured workflow or an unperformed checklist is not evidence of a passing run.
