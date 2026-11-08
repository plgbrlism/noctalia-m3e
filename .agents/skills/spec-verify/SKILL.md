---
name: spec-verify
description: Run one verification gate VG-NN and write facts/VG-NN-<topic>.md. A milestone cannot begin until its blocking gates have pass verdicts. Use before kickoff of any milestone.
---

# spec-verify

## Inputs
- Gate ID: `VG-00` … `VG-08`

## Load (only these)
1. Row for `<GateID>` in `spec/01-gates.md`
2. Invariants from `AGENTS.md §Invariants` that the gate protects
3. `noctalia-v5-index.md` entries for the gate's subsystem
4. The specific files the index points to (open them, read them)

## Do not load
- Other gates
- Other subsystems
- Full `ARCHITECTURE.md`
- `spec/04..08`
- Any other `src/` or `lib/` file

## Output
Write `facts/<GateID>-<topic>.md`:
```
# <GateID> — <topic>
Verdict: pass | fail | partial
Baseline commit: <sha>
Date: <YYYY-MM-DD>

## Evidence
- <claim> — `<path>:<line-range>`

## If fail
- Smallest change to unblock:
- Proposed spec amendment (if gate invariant needs revision):
```

## Stop
After writing the facts file. Do not begin implementation. Do not load the
next gate. Return control to human.
