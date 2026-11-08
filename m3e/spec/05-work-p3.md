# 5. Work breakdown — P3 and Opt

Full table (all tiers) lives in `m3e/spec.md` §5. This file is P3 and Opt only.

| ID | Requirement | Target | Acceptance | Tier | Effort |
|---|---|---|---|---|---|
| TYP-001 | `font_variations` plumbing (gate VG-03) | Label, CairoTextRenderer | Axis applied or explicit warning; fallback fonts safe | P3 | M |
| TYP-002 | M3 type-scale tokens in `lib/ui/style.h`; cap-band centering and fractional positions preserved | style.h, Label | Existing Label tests pass | P3 | M |
| CHO-001 | Stagger utility, default 50 ms tunable, off under reduce | AnimationManager | Interruption-safe, INV-03 compliant | P3 | S |
| CHO-002 | Hero-first, default 125 ms tunable (100-150), off under reduce | AnimationManager | Focal element first; cancel restores state | P3 | M |
| PLG-001 | Plugin token-read API | src/scripting | INV-08 preserved | Opt | M |
| TEST-001 | Golden visual tests | CI | Threshold enforced | Opt | M |
| TEST-002 | RSS and frame-time budgets (RSS delta 10 MB max, 60fps 1080p) | bench | Within budget | Opt | M |
