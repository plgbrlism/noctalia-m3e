# Upstream diff — overview

Comparison of `fork/m3e` against upstream `origin/main`.

| Field | Value |
|---|---|
| Upstream ref | `origin/main` |
| Fork ref | `fork/m3e` |
| Local branch | `m3e` (fork head d10812ce1) |
| Merge base | 76637f9d4 |
| Scope | 122 files, +4720 / -591 lines |

This directory decomposes the full diff by subsystem. Each page lists touched files and the intent of the change. Narrative rationale lives in `m3e/spec.md` and `m3e/spec/12-revisions.md`.

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

Generated 2026-11-17. Refresh after each upstream rebase with `git diff --stat origin/main...fork/m3e`.
