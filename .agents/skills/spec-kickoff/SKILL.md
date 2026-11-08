---
name: spec-kickoff
description: Start a milestone — produce ordered work-item list with critical path and effort. Requires blocking VG gates for that milestone to have pass verdicts in facts/.
---

# spec-kickoff

## Inputs
- Milestone ID: `M1` … `M6`

## Precondition
Read `facts/` filenames. Cross-check against `spec/09-milestones.md` row
for this milestone ("blocks" column). If any blocking gate lacks a
`facts/VG-NN-*.md` file, stop and report — do not proceed to design.

## Load (only these)
1. Milestone row for `<M>` in `spec/09-milestones.md`
2. `spec/05-work-<tier>.md` filtered to rows tagged for this milestone
3. `noctalia-v5-index.md` entries referenced by those work items
4. Invariants from `AGENTS.md §Invariants` referenced by those items

## Do not load
- Other milestones
- Other work items
- `spec/04..08` beyond what §5 rows reference

## Output
1. Ordered work-item list: ID | tier | effort | files | VG status
2. Critical path (what unblocks what)
3. Total estimated effort vs milestone budget
4. Per-item acceptance criteria (verbatim from §5)

## Stop
After output. Wait for human to pick first work item.
