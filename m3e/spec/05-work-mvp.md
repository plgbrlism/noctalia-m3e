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
| COL-001 | Control color-role audit (container/on-container, AA, no hardcoded hex) | controls, palette.h | Contrast AA | MVP | M |
| COL-002 | Expressive high-chroma scheme alias (Material Color Utilities variant) | src/theme | Higher-chroma palette selectable | MVP | S |
| CFG-001 | `[motion]` schema: scheme, reduce, per-token overrides; defaults expressive/false | config_types.* | Invalid token errors, no partial apply | MVP | S |
