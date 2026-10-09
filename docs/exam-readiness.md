# Exam readiness

[Documentation index](index.md) · [Validation evidence](validation.md) · [Improvement report](improvement-report.md)

The `sac4ap` branch resumes the existing Computer Graphics project for Software
and Computing for Applied Physics. It adds documentation, tests and verification
infrastructure while keeping all original commits and production behavior.

## Criteria and evidence

| Course criterion | Evidence in this branch | Remaining limit/action |
| --- | --- | --- |
| Public version-controlled project | Existing GitHub repository; `sac4ap` descends from original `main` tip `20ae124` with all 263 original commits. | Publish/access the exam branch, not just the default `main` landing page. |
| Clear commit history | New descriptive, focused commits at the actual resumption date; no history rewrite or fabricated dates. | Follow the same discipline for later changes. |
| Runs on another machine | Explicit Windows x64/MSVC/OpenGL requirements and step-by-step clean checkout build commands. | Fresh build required; keep resources in the configuring checkout. Other operating systems are not claimed. |
| Understandable code and algorithms | [Architecture](architecture.md), [API](api.md), targeted contract/rationale comments, and algorithm assumptions. | Existing naming/style/global-state design is preserved; proposed refactors remain in the report. |
| English documentation available from landing page | README and [documentation index](index.md) link tutorial, how-to, reference, explanation, tests and limitations. | Optional Doxygen HTML is generated locally with `doxygen Doxyfile`; it is not a deployed site. |
| Executable, separated tests | Catch2/CTest tests compile original code; standard-library Python tests check tools and packaged data. | Graphics/gameplay integration is only partially automated; see [testing](testing.md). |
| Expected results asserted | Analytic geometric/kinematic expectations, tolerant float comparisons, deterministic properties and negative fixtures. | Unchecked invalid inputs are documented preconditions, not claimed validation features. |
| Automation on a clean environment | Windows Debug/Release CI, explicit CMake/Python versions, saved JUnit/CTest diagnostics. | Remote CI results and manual checks must be distinguished from local build/test results. |
| Reproducibility | Listed dependencies, asset preflight and unchanged-source guard with immutable baseline. | Original `stb master` dependency is still mutable; no whole-project coverage percentage is claimed. |

## Boundaries of this adaptation

The task's constraint forbids production fixes and refactoring. The independent
test project and helper scripts therefore sit outside the application build.
Only useful comments are changed in existing C++ sources; the original root
CMake files, resource files and bundled GLAD are preserved.

This branch improves usability and verification substantially, but documentation
and a passing scoped suite cannot prove every user-visible path correct. The
course explicitly asks to test exposed functionality. Full coordination inside `Game`,
model import, resource initialization, callbacks and menus remains outside the exercised unit paths. Player, cat, projectile and collision CPU behavior is now tested using the real implementations. Pure
rendering may be assessed visually; nonvisual processing in those classes is
still a testing gap. The [report](improvement-report.md) explains how to close it
if production changes are accepted later.

The demonstration has a fixed scenario and built-in help/menu controls. No
command-line configuration interface has been invented or claimed. Building and
running the documented demo does not require editing source files.

## Before presenting

- Confirm the use of this graphics/game project with the examiner if it has not
  already been discussed. The exam guidelines allow sharing a project with
  another exam, and ask for prior discussion of its suitability.
- Give the examiner the URL of the **`sac4ap` branch** and a specific commit.
- Run Debug/Release checks and the [manual graphics checklist](manual-testing.md)
  on the intended demonstration machine. Record actual outcomes and driver info.
- Check the CI run for the exact submitted commit. Do not present a workflow file
  as proof of a green remote run.
- Decide whether to accept any improvements in the separate report. Keep that
  decision explicit rather than silently changing the original implementation.

## Course references

The authoritative requirements are the
[exam details](https://unibodifabiophysics.github.io/programmingCourseDIFA/Lesson_00_details_about_the_exam.html),
[version control lesson](https://unibodifabiophysics.github.io/programmingCourseDIFA/Lesson_03_version_control.html),
[testing lesson](https://unibodifabiophysics.github.io/programmingCourseDIFA/Lesson_04_testing.html),
and [documentation/API lesson](https://unibodifabiophysics.github.io/programmingCourseDIFA/Lesson_AF_08_Documentation_and_API.html).
This checklist interprets those requirements for the existing application; it is
not an assurance of examiner approval or a particular grade.