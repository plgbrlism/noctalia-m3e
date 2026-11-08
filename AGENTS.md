# Noctalia M3E Fork — Agent Rules

## Identity
- Fork of noctalia v5, branch m3e/rev5
- Converting to Material 3 Expressive per spec in spec/
- Baseline commit: <SHA>
- Solo fork, additive changes

## The one rule
Never read src/ or lib/ except through noctalia-v5-index.md.

## Invariants (must preserve in every edit)
<table of INV-*>
<table of PHASE-*>
<table of MOT-*>
<table of SHP-*>

## File map
- spec/00-invariants.md — canonical invariant list (this section is a mirror)
- spec/01-gates.md — VG gates
- spec/04-motion.md — motion tokens
- spec/05-shape.md — shape model
- spec/05-work-mvp.md — MVP work items
- spec/05-work-p2.md — P2 work items
- spec/05-work-p3.md — P3 work items
- spec/06-config.md — config schema
- spec/07-color.md — color rules
- spec/08-tests.md — test matrix
- spec/09-milestones.md — M0..M6
- spec/10-risks.md — risks
- spec/12-revisions.md — revision log
- noctalia-v5-index.md — subsystem → file map (replaces ARCHITECTURE.md)
- facts/VG-NN-*.md — verification gate outputs
- WORKLOG.md — append-only edit log

## Session protocol
1. New session: run spec-bootstrap skill
2. Context >65%: run spec-handoff skill, then clear
3. Resume: run spec-bootstrap skill

## Hard rules
- One file per edit turn
- Never modify ARCHITECTURE.md
- Never modify spec/*.md directly — use spec-amend skill
- Append only to WORKLOG.md
- Load only files named in the active skill's "Load" section
- Read files with explicit `read path` — do not use search/glob
- Do not use @ syntax — use explicit reads

## Skills
- spec-bootstrap — cold start, resume, initial index generation
- spec-verify — run one VG gate
- spec-kickoff — start a milestone
- spec-design — design a work item (no edits)
- spec-edit — edit one file toward one work item
- spec-test — run tests, fix failures
- spec-review — review a diff against invariants
- spec-handoff — produce handoff block before context clear
- spec-amend — propose spec/index amendment
