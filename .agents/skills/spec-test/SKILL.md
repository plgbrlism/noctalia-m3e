---
name: spec-test
description: Run one failing test toward a work item and fix one assertion. One assertion per turn. Never bundle fixes.
---

# spec-test

## Inputs
- Work item ID
- Test file path
- Failing assertion (assertion text or line number)

## Load (only these)
1. The `AT-*` row from `spec/08-tests.md` for this test
2. The test file
3. The last diff applied (from `WORKLOG.md`)
4. The invariant the test verifies

## Do not load
- Other tests
- Other invariants
- Other files

## Output
1. Root cause (one sentence)
2. Minimal production-code fix diff (one file)
3. Test re-run command
4. Expected outcome (pass/fail + new evidence)

## Stop
After diff. Human runs the test. On pass, they invoke spec-edit for the
next file or spec-review for the accumulated diff.
