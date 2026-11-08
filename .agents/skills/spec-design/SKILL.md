---
name: spec-design
description: Produce a design for one work item — interfaces, file list, tests to pass, invariants at risk. Do not edit files. Human must approve before spec-edit.
---

# spec-design

## Inputs
- Work item ID: e.g. `MOT-001`, `SHP-001`, `COL-001`, `CFG-001`

## Load (only these)
1. Full row for this work item from `spec/05-work-<tier>.md`
2. Invariants from `AGENTS.md §Invariants` listed as dependencies
3. The spec sub-section it references (e.g. `spec/04-motion.md` for MOT-*,
   `spec/05-shape.md` for SHP-*, `spec/06-config.md` for CFG-*)
4. `noctalia-v5-index.md` entries for the files it will touch
5. The matching `AT-*` rows from `spec/08-tests.md`

## Do not load
- Other work items
- Other spec files
- Any `src/` or `lib/` file
- `ARCHITECTURE.md`

## Output
1. **Interfaces to add or change** — header signatures only, no bodies
2. **Files to edit** — path | confidence (certain / likely / <verify>)
3. **Tests it must pass** — pull `AT-*` rows verbatim
4. **Invariants at risk** — ID | how preserved
5. **Open questions** for human

## Stop
After output. Do not edit files. Do not begin spec-edit.
