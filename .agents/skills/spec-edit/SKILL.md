---
name: spec-edit
description: Edit exactly one source file toward an approved design. One file per turn. Never multi-file. Requires prior spec-design approval.
---

# spec-edit

## Inputs
- Work item ID
- Target file path (one file only)
- Approved design from previous spec-design turn

## Load (only these)
1. This work item's row from `spec/05-work-<tier>.md`
2. Invariants from `AGENTS.md §Invariants` listed in the design's
   "invariants at risk"
3. `noctalia-v5-index.md` entry for the target file
4. The single `ARCHITECTURE.md` section the file belongs to
   (read by section header, not the whole file)
5. The target file itself

## Do not load
- Other files in the subsystem
- Other work items
- `spec/04..08` beyond what the design named
- Full `ARCHITECTURE.md`

## Output
1. Minimal diff for the target file only
2. One-line WORKLOG entry to append:
   `[<item-id>] <file> <what> pending-test`
3. Any invariant touched by the diff (ID + one line)

## Stop
After the diff. Do not run tests. Do not edit a second file.s
