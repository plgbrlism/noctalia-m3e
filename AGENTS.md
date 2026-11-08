# Noctalia M3E Fork — Agent Rules

Solo fork of noctalia v5 → Material 3 Expressive. Changes additive in `lib/`.
Branch: `m3e/rev5`. Spec lives in `m3e/spec/`.

## The one rule that overrides everything

**Never read `src/` or `lib/` except through `m3e/noctalia-v5-index.md`.**
If a needed file is not in the index, stop and report — do not guess a path.

## Invariants (must hold in every edit)

### Architecture
- INV-01: `lib/` never includes `src/`; `lib/render` draws generic primitives only
- INV-02: Shell/widget code never creates raw scene nodes; controls own them
- INV-03: Phases strict: tick in PrepareFrame, state sync in Update, geometry in Layout, paint only in Render
- INV-04: No layout or scene-graph mutation during Render
- INV-05: Panel/bar policy stays in shell; shader enums carry no policy names
- INV-06: Palette/ColorRole/ColorSpec in `lib/ui/palette.h` and `lib/ui/style.h`
- INV-07: Config schema in `src/config/config_types.*`; overrides in `config_overrides.cpp`; GUI writes `settings.toml`; runtime state in `state.toml` via StateStore
- INV-08: Plugins are trusted Luau composition only; no renderer or animation-core access
- INV-09: Setters schedule via requestUpdate/requestLayout/requestRedraw/requestFrameTick; never rebuild synchronously

### Motion
- PHASE-001: Springs tick in PrepareFrame; retarget in Update; geometry in Layout; shader reads resolved uniforms in Render
- PHASE-002: Spatial (position/size/radius/morph amount) → requestFrameTick; discrete style switch → requestLayout; color/opacity/elevation → requestRedraw
- MOT-SPRING-001: Springs are damping ratio ζ, mass 1; convert `c = 2·ζ·√k` if AnimationManager uses absolute damping
- MOT-INT-001: Closed-form damped-spring integration; no easing-curve sampling. State is (x, v, target, t_since_retarget); retarget preserves (x, v)
- MOT-DT-001: PrepareFrame clamps dt to 32 ms
- MOT-IDLE-001: Zero wakeups at idle; spring snaps and stops requestFrameTick when |x−target|<εx and |v|<εv
- MOT-REDUCE-001: `[motion].reduce=true` sets ζ=1.0 for every spatial token (keeps stiffness); disables choreography delays
- MOT-SCHEME-001: On scheme change, in-flight springs retarget to same target with new parameters, preserving (x,v)

### Shape
- LIB-001: MotionScheme, SpringToken, ShapeName, ShapeProfile, ShapeController live in `lib/`; shell config maps tokens, never redefines them
- SHP-EXT-001: Shape extends generic rect primitive in `lib/render/programs/`; no shell-only RenderExtension
- REF-001: ShapeName list matches m3e `ShapeName.ts` exactly (35 names, no `scallop`)
- SHP-CLAMP-001: Corner radius clamped to [0, min(w,h)/2]; morph `amount` clamped to [0,1]; only position/size may overshoot
- SHP-DAMAGE-001: Damage rect includes spring overshoot beyond rest rect
- SHP-SCALE-001: Radii and shape uniforms are logical px × output scale; verified at 1.25× and 1.5×
- SHP-BOUNDS-001: Outer bounds fixed during press morph; only radii animate; padding recomputed to preserve label

### Settle epsilons (defaults)

| Quantity | εx | εv |
|---|---|---|
| Position/size/radius | `0.5/scale` logical px | `1.0/scale` px/s |
| Morph amount | 0.001 | 0.001 /s |
| Color/opacity | 0.002 | 0.002 /s |

## File map

| File | Purpose |
|---|---|
| `m3e/spec/01-gates.md` | VG-00..VG-08 |
| `m3e/spec/04-motion.md` | Motion tokens |
| `m3e/spec/05-shape.md` | Shape model |
| `m3e/spec/05-work-mvp.md` | MVP work items |
| `m3e/spec/05-work-p2.md` | P2 work items |
| `m3e/spec/05-work-p3.md` | P3 + Opt work items |
| `m3e/spec/06-config.md` | Config schema |
| `m3e/spec/07-color.md` | Color rules |
| `m3e/spec/08-tests.md` | Test matrix |
| `m3e/spec/09-milestones.md` | M0..M6 |
| `m3e/spec/10-risks.md` | Risks + DoD |
| `m3e/spec/12-revisions.md` | Revision log |
| `m3e/noctalia-v5-index.md` | Subsystem → file map (replaces ARCHITECTURE.md) |
| `m3e/facts/VG-NN-*.md` | Per-gate evidence |
| `m3e/WORKLOG.md` | Append-only edit log |

## Skills index

| Skill | When | Produces |
|---|---|---|
| spec-bootstrap | Cold start / resume after handoff | Session state summary |
| spec-verify | Run one VG gate | `m3e/facts/VG-NN-*.md` |
| spec-kickoff | Start a milestone | Ordered work-item plan |
| spec-design | Design one work item (no edits) | Interfaces + files + tests |
| spec-edit | Edit ONE file toward ONE work item | Minimal diff |
| spec-test | Fix ONE failing assertion | Fix diff |
| spec-review | Review diff vs invariants | Pass/fail table |
| spec-handoff | Before context clears | ≤400-word handoff block |
| spec-amend | Propose spec/index amendment | Diff + impact list |

## Session protocol

1. New session: `spec-bootstrap`. Nothing else until reviewed.
2. Context >65%: `spec-handoff`, human clears, resume with `spec-bootstrap` + handoff.
3. Before first work item of a milestone: blocking VGs must have `m3e/facts/VG-NN-*.md` with `Verdict: pass`.
4. Per work item: `spec-design` → human approves → `spec-edit` per file → `spec-test` per assertion → `spec-review` on diff.
5. Spec amendment: only via `spec-amend`.

## Hard rules

- One file per `spec-edit` turn. Multi-file edits are a bug.
- Read files with explicit `read <path>`. No glob, no grep, no `@`.
- Never modify `ARCHITECTURE.md`.
- Never modify `m3e/spec/*.md` directly — use `spec-amend`.
- Append-only `m3e/WORKLOG.md`.
- Never read spec files not on the active skill's load list.
- Never load `m3e/spec/10-risks.md` unless explicitly asked.
- If a needed file is not in `m3e/noctalia-v5-index.md`, stop and report.
