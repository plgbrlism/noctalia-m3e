---
name: spec-handoff
description: Produce a ≤400-word handoff block before context clears. Run when context >65%.
---

# spec-handoff

## Load
Nothing new. Use context already present. Do not read any file.

## Output
≤400 words covering:
1. Current milestone + work item ID
2. Files modified (paths only, no diffs)
3. Tests passing / failing (AT-* IDs)
4. Invariants to re-check on resume (IDs)
5. Next 3 concrete actions
6. Open questions for human

## Stop
After output. Human will clear context and run spec-bootstrap with this
block pasted.
