# VG-08 — spring numbers vs Compose MotionScheme.kt

Verdict: pass
Baseline commit: aa56af82e
Date: 2026-11-08

## Evidence
- Expressive spatial: `ExpressiveMotionTokens.kt:26-31` = fast 0.6/800, default 0.8/380, slow 0.8/200; matches `m3e/spec/04-motion.md:7-9`.
- Expressive effects: `ExpressiveMotionTokens.kt:24-33` = fast 1.0/3800, default 1.0/1600, slow 1.0/800; matches `m3e/spec/04-motion.md:10-12`.
- Standard spatial: `StandardMotionTokens.kt:24-29` = fast 0.9/1400, default 0.9/700, slow 0.9/300; matches `m3e/spec/04-motion.md:7-9`.
- Standard effects: `StandardMotionTokens.kt:22-31` = fast 1.0/3800, default 1.0/1600, slow 1.0/800; matches `m3e/spec/04-motion.md:10-12`.
- Wiring: `MotionScheme.kt:139-170` (standard) and `:203-234` (expressive) pass each pair to `spring(dampingRatio=..., stiffness=...)`; mass default is 1, matching MOT-SPRING-001.
