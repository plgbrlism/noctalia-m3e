# VG-07 — token and shape names vs m3e and Compose

Verdict: partial (names assumed correct per spec rev5; not diffed against live sources)
Baseline commit: 8be68ffd6
Date: 2026-11-08

## Evidence
- Motion token names in m3e/spec/04-motion.md: spatial.fast/default/slow, effects.fast/default/slow
- Internal IDs: M3E-<SCHEME>-<FAMILY>-<SIZE> — matches m3e ThemeElement.ts pattern
- ShapeName list in m3e/spec/05-shape.md §3.1: 35 names, no `scallop` — matches m3e ShapeName.ts
- Not diffed live against matraic/m3e or Compose MotionScheme.kt this session

## If fail
- Smallest change: diff §3.1 and §4 against upstream sources before MOT-001 implementation begins
- Does not block M1 design; blocks MOT-001 code
