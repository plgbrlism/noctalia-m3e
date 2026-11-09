# 5. Work breakdown — MVP

Full table (all tiers) lives in `m3e/spec.md` §5. This file is MVP only.

| ID | Requirement | Target | Acceptance | Tier | Effort |
|---|---|---|---|---|---|
| MOT-001 | MotionScheme (standard/expressive) + SpringToken resolver, both schemes populated, expressive default | lib animation core | 12 tokens resolve; unknown token rejected | MVP | M |
| MOT-002 | Add animateSpring() state machine alongside existing animate(); state is (x, v, target, zeta, k, t_since_retarget). Leave animate() untouched. | AnimationManager | 200 ms stall does not explode; effects overshoot = 0 | MVP | M |
| MOT-003 | Retargeting preserved with tokens | AnimationManager | Mid-flight retarget does not snap | MVP | M |
| MOT-004 | Migrate MVP-tier callers to animateSpring(); leave remaining callers on animate() until M3. | lib/ui/controls | No scalar-speed M3E transitions | MVP | M |
| MOT-005 | Settle detection with eps table (MOT-IDLE-001) | AnimationManager | Zero requestFrameTick after settle | MVP | S |
| MOT-006 | `[motion].reduce` (MOT-REDUCE-001) | AnimationManager, config | Spatial overshoot gone when true | MVP | S |
| SHP-001 | Animated per-corner radii on rect primitive with clamp and overshoot damage | lib/render/programs, controls | Press morph works; radius in `[0, min(w,h)/2]`; no clip | MVP | M |
| SHP-002 | Fractional-scale plumbing for radii (SHP-SCALE-001) | RenderContext, rect program | Correct at 1.25x and 1.5x | MVP | S |
| COL-001 | Contrast checker test against src/theme/contrast.h (WCAG 2.1 formula) | tests/color_contrast_test.cpp | AT-COL-01 passes: formula correct on known pairs (black/white → 21, identity → 1, symmetry, mid-gray boundary). No control edits needed. | MVP | S |
| COL-002 | Expose SchemeExpressive as [theme].wallpaper_scheme = "m3-expressive" | src/theme/scheme.h, src/theme/m3_schemes.cpp, settings dropdown | "m3-expressive" accepted in config; resolves to SchemeExpressive Palette; existing schemes unchanged. SchemeExpressive already vendored at third_party/material_color_utilities/cpp/scheme/scheme_expressive.cc. | MVP | S |
| CFG-001 | `[motion]` schema: scheme, reduce, per-token overrides; defaults expressive/false | config_types.* | Invalid token errors, no partial apply | MVP | S |
