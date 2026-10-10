# Contributing

[Documentation index](index.md) · [Testing](testing.md)

Work on **sac4ap-dev** for production improvements. `main` preserves the original project and `sac4ap` the behavior-compatible exam adaptation. Keep those tips unchanged. Retain ancestry/dates: create descriptive commits at the actual work date, without rewriting old history or fabricating activity.

Specify a behavior contract and add meaningful regressions/negative cases. Update implementation, rationale comments, API/tutorial and validation together. Asset edits need separate explicit scope; the current guard preserves original `res/` and `external/` contents.

```powershell
.\scripts\run_checks.ps1 -Configuration Debug -BuildApplication
.\scripts\run_checks.ps1 -Configuration Release -BuildApplication
python scripts/check_history_preserved.py
git diff --check
```

Use a real desktop for `--graphics-check` and the [manual checklist](manual-testing.md). Packaging changes also require `package_release.ps1`, byte verification and relocated launch. Native jobs in [sac4ap-dev.yml](../.github/workflows/sac4ap-dev.yml) verify their compiler/dependencies; inspect the exact submitted commit.

Stage intended files only. Do not commit dependency downloads, binaries, build outputs, private paths, logs/caches. Write English documentation tied to actual interfaces; explain units/errors/lifetimes. Comments should explain contracts/rationale rather than restate statements.

Separate executed evidence from expected outcomes. Update characterization cases only when an approved contract changes; old behavior/evidence remains on the original branch. Understand failures before changing assertions. Project/asset licenses require the owner's decision/evidence.
