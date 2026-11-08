# VG-01 — springs

Verdict: fail (velocity state absent; additive path viable)
Baseline commit: d4ca76442
Date: 2026-11-08

## Evidence
- `Animation` struct is duration tween — `lib/render/animation/animation.h:14-24` (no velocity field)
- `AnimationManager::animate(from, to, durationMs, easing, setter)` — `lib/render/animation/animation_manager.h:22`
- No spring/damping/velocity/zeta anywhere in `lib/render/animation/` (rg clean)
- 31 `.animate(` callers across `src/` + `lib/`
- `MotionService` only exposes global `enabled` + `speed` — `lib/render/animation/motion_service.h`

## If fail
- Smallest change: add `animateSpring()` beside `animate()` in
  AnimationManager; new state (x, v, target, zeta, k, t_since_retarget);
  leave `animate()` untouched
- Proposed amendment: MOT-002 → "add spring state machine" (not "extend
  tween"); MOT-004 → "migrate MVP-tier callers only, defer rest"
- MOT-SPRING-001 conversion moot until springs exist; keep rule for
  user-tunable config
