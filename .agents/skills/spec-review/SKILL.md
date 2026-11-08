---
name: spec-review
description: Review a diff against invariants only. Forces per-invariant pass/fail table. No style comments.
---

# spec-review

## Inputs
- The diff (pasted or path under `facts/`)
- Work item ID

## Load (only these)
1. `AGENTS.md §Invariants` (re-read)
2. The diff itself

## Do not load
- The codebase
- `spec/*` beyond invariants
- The work item row

## Output
Table: `Invariant | Pass/Fail | Evidence (line in diff)`

Then:
- Phase discipline: PHASE-001, PHASE-002
- Library boundary: INV-01, LIB-001, SHP-EXT-001
- Request-type check: requestRedraw vs Layout vs FrameTick
- Bound check: SHP-CLAMP-001, SHP-BOUNDS-001

Flag only violations. Do not praise. Do not comment on style.
If verdict is "all pass", say exactly: "All invariants pass. Mergeable."

## Stop
After the table. Do not propose further edits.
