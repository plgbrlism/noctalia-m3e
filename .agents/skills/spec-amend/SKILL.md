---
name: spec-amend
description: Propose one amendment to spec or index. Never edits directly. Produces a diff the human applies.
---

# spec-amend

## Inputs
- Target: `spec/<file>.md` §<section> OR `noctalia-v5-index.md` entry
  <subsystem> OR `AGENTS.md §Invariants §<ID>`
- Proposed change (one sentence)

## Load (only these)
1. The target file's relevant section (read a range, not the whole file)
2. The IDs of work items and VG gates that reference the target

## Do not load
- The codebase
- Other spec files

## Output
1. Exact diff (unified format)
2. Work items affected (by ID)
3. VG gates affected (by ID)
4. Whether any milestone estimate changes and why
5. Whether `spec/08-tests.md` needs a new `AT-*` row

## Stop
After output. Human applies or rejects. Do not edit any file.
