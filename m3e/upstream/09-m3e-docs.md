# Upstream diff — m3e docs and skills

| Path | Δ | Change |
|---|---|---|
| `AGENTS.md` | +102 new | Fork agent rules, invariants, spec workflow, file-map rule (`m3e/noctalia-v5-index.md`) |
| `.agents/skills/spec-*.md` | +298 new | 10 skills: bootstrap, verify, kickoff, design, edit, test, review, handoff, amend, plus caveman helpers |
| `m3e/spec.md` | +285 new | Full rev5 spec (ghost m3e index), rev8 appended |
| `m3e/spec/*.md` | +319 | Gates, motion, shape, work tiers, config, color, tests, milestones, risks, revisions |
| `m3e/facts/VG-*.md` | +105 | Gate evidence (VG-00,01,02,07,08,09); VG-09 amended post-exposure |
| `m3e/WORKLOG.md` | +70 | Edit log; git history appended through d10812ce1 |
| `m3e/spec/*`, `m3e/upstream/*`, `m3e/noctalia-v5-index.md` | rev9 | Document UI polish pass; direct maintainer bulk update per approval |
| `m3e/noctalia-v5-index.md` | +106 | Subsystem → file map, replaces ARCHITECTURE.md for reads |

These documents are fork-only. The upstream branch has no `m3e/` tree and no spec workflow. Per AGENTS.md, spec changes normally go through `spec-amend`; rev8 was applied as an explicit maintainer-directed bulk update.
