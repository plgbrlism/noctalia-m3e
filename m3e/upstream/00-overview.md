# Upstream diff — overview

Current branch topology and patch status for the vespertine fork against noctalia upstream.

| Field | Value |
|---|---|
| Upstream remote | `upstream` = `noctalia-dev/noctalia` |
| Upstream ref | `upstream/main` at `230768446` |
| Product branch | `master` at `e36a59f36`, tracks `origin/master` |
| Product remote | `origin` = `plgbrlism/vespertine`, default `master` |
| Staging branch | `garden` at `d130863c9`, tracks `origin/garden` |
| Upstream mirror | `upstream/noctalia` at `230768446`, tracks `upstream/main` |
| Merge base (garden vs upstream) | `76637f9d4` |
| Raw compare, master vs upstream | 165 ahead, 106 behind |
| Raw compare, garden vs upstream | 162 ahead, 106 behind |

| Patch status of upstream vs garden | Count |
|---|---:|
| Patch-identical, old cherry-picks | 64 |
| Patch-different | 38 |
| Patch-different with same subject in garden | 19 |
| No same subject in garden | 19 |

This directory decomposes the fork diff by subsystem. Narrative rationale lives in `m3e/spec.md` and `m3e/spec/12-revisions.md`.

| Page | Scope |
|---|---|
| `01-animation-motion.md` | Spring engine, token resolver, motion service, frame loop |
| `02-ui-controls.md` | Button, flex, segmented, stepper, toggle, slider, Style tokens |
| `03-config.md` | Config types, schema, overrides, migrations, app services |
| `04-panel-system.md` | Panel reveal variants and attached/detached chrome |
| `05-shell-surfaces.md` | Bar, dock, control center, launcher, OSD, tooltip, wallpaper and other shell surfaces |
| `06-settings.md` | Settings window, sheets, sidebar, registry |
| `07-theme-color.md` | M3 scheme exposure, palette generation, translations |
| `08-tests.md` | New test suites added by the fork |
| `09-m3e-docs.md` | AGENTS.md, spec skills, m3e specs/facts/worklog |

Raw ahead/behind counts commit hashes. Cherry-picks create new hashes, so the numbers overstate unique work. Patch status is the useful signal.

Generated 2026-11-20. Refresh after each upstream sync with `git fetch upstream && git rev-list --left-right --count master...upstream/main`.
