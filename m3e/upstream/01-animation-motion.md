# Upstream diff — animation and motion

| File | Δ | Change |
|---|---|---|
| `lib/render/animation/motion_scheme.{h,cpp}` | +157/+60,+60 | New: `MotionScheme` (standard/expressive), `SpringToken` resolver, `SpringSpec`, `MotionConfig`, settle epsilons, reduce branch |
| `lib/render/animation/animation_manager.{h,cpp}` | +41/-8,+198/-3 | `SpringState` + `animateSpring()` closed-form integration, `retargetSpring`/`retargetSpringSpec`, `FrameRequest`, `tickSprings`, `springValue`, owner-based cancel, `hasAnyActive` |
| `lib/render/animation/motion_service.{h,cpp}` | +18/-4 | `setMotionConfig`/`motionConfig`/`resolve(SpringToken)`, speed² multiplier in custom mode |
| `lib/wayland/surface.cpp` | +3 | Wire spring tick into frame loop |
| `lib/meson.build`, `meson.build` | +1/+12 | Register new sources and tests |

Upstream had only duration tweens (`animate()`), a global enabled/speed pair in MotionService, and no token table. The fork adds a parallel spring path without removing `animate()`.

Key design points:
- Springs carry `(x0, v0, x, v, target, zeta, k, elapsed)` and preserve `(x, v)` on retarget.
- Settle uses epsilons; settled springs stop requesting frames (MOT-IDLE-001).
- Expressive/standard tables match Compose `MotionScheme.kt` (see `m3e/facts/VG-08-spring-numbers.md`).
- Reduce motion swaps spatial tokens to effects tokens.

Work items: MOT-001..006, CFG-001. Gates: VG-01, VG-08.
