# 2. Gap summary

| Component | Gap | Priority |
|---|---|---|
| AnimationManager | No tokenized schemes, settle detection, dt clamp, closed-form solve, reduced-motion | P0 |
| Rect primitive | No per-corner animated radii path with clamp, overshoot damage, fractional-scale plumbing | P0 |
| Shape system | No ShapeName, no r(theta) profile, no star-convex handling | P2 (post-MVP) |
| Color | No expressive high-chroma alias; role audit incomplete | P0 |
| Controls | Interactive transitions use scalar speed; no press radii morph | P0 |
| Components | No connected button group, split button, wavy progress | P2 |
| Typography | No `font_variations`, no type-scale tokens | P3 |
| Choreography | No stagger/hero-first | P3 |
| Config | No `[motion]`, `[shape]`, `[typography]`, `[choreography]` | P0 (motion only), rest later |
| Plugins | Closed to renderer/animation (confirmed by design) | none |

# 9. Milestones

| Milestone | Scope | Blocks (gates) | Exit | Time |
|---|---|---|---|---|
| M0 | VG-00 to VG-08 | — | Baseline, spring convention, uniform path, numbers confirmed | 2-3 days |
| M1 | MOT-001 to 006, CFG-001 | VG-00, VG-01, VG-07, VG-08 | Tokens live, idle verified, controls migrated | ~1 week |
| M2 | SHP-001, SHP-002 | VG-02 | Press radii morph, clamp, damage, fractional scale | ~1 week |
| M3 | COL-001, COL-002 | VG-04 | Expressive palette, role audit | ~1 week |
| MVP | M0 to M3 | — | Shippable expressive feel | 4-6 weeks total |
| M4 | COL-003, SHP-003 to 007 | VG-05 | r(theta) shapes, controller, tier-1 wiring | +4-5 weeks |
| M5 | CMP-001 to 003 | — | Expressive components | +3 weeks |
| M6 | TYP, CHO | VG-03 | Type scale, variable fonts, choreography | +2-3 weeks |
| M7 | Dux preset adoption (rev6) | REV-001..005, SHP-008, MOT-007 | All reversions landed, `[shell].shape` and `[motion].scheme` accepted in config, no legacy float keys in schema | +1-2 weeks |
| M8 | Design system polish (DES-004 preset layer + DES-005 panel motion) | DES-004, DES-005 | All 5 new config fields parse and gate correctly; GUI shows all presets under Expressive; 4 panel motion variants ship and pass AT-PANEL-01..04; reduce motion respected | +2-3 weeks |
