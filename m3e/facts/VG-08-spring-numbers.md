# VG-08 — spring numbers vs Compose MotionScheme.kt

Verdict: partial (numbers from rev5 spec, not re-diffed)
Baseline commit: 8be68ffd6
Date: 2026-11-08

## Evidence
- Expressive: spatial 0.6/800, 0.8/380, 0.8/200; effects 1.0/3800, 1.0/1600, 1.0/800
- Standard: spatial 0.9/1400, 0.9/700, 0.9/300; effects same as expressive
- Per m3e spec rev5 §4 (Provisional until this gate). Not re-diffed against
  Compose source this session.
- Single token table means correction is a one-line fix

## If fail
- Smallest change: update one table in m3e/spec/04-motion.md
- Does not block M1 design; blocks MOT-001 code
