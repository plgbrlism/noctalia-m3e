---
name: spec-bootstrap
description: Cold start of a session or resume after handoff. Establishes current state without loading the codebase. Run first on every new session.
---

# spec-bootstrap

## Inputs
- Optional: handoff block pasted by human (if resuming)

## Load (only these)
1. `AGENTS.md` (already loaded — re-read §Invariants §Skills index)
2. `spec/09-milestones.md` (full — you need current milestone + exit criteria)
3. Last 20 lines of `WORKLOG.md`
4. `facts/` directory listing (filenames only, not contents)
5. First 15 lines of `noctalia-v5-index.md` (subsystem list only)
6. Handoff block if present

## Do not load
- Any `spec/04..08` file
- Any `src/` or `lib/` file
- Full `noctalia-v5-index.md`
- `ARCHITECTURE.md`
- Any `facts/*.md` content

## Output
1. Current milestone (from WORKLOG or handoff; say "unknown" if neither
   disambiguates)
2. Gates passed so far (from `facts/` filename list — infer pass from
   presence, but say "verify content" if ambiguous)
3. Last 3 WORKLOG entries verbatim
4. Recommended next skill: one of `spec-verify <VG>`, `spec-kickoff <M>`,
   `spec-design <item>`, `spec-handoff`
5. Open questions for human

## Stop
After output. Wait for human direction. Do not propose work.
