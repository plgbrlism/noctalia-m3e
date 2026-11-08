# VG-07 — token and shape names vs m3e and Compose

Verdict: pass
Baseline commit: aa56af82e
Date: 2026-11-08

## Evidence
- ShapeName: `packages/web/src/shape/ShapeName.ts:3-37` lists 35 names in the same order as `m3e/spec/05-shape.md:15`; includes `very-sunny`, no `scallop`.
- Motion schemes: `packages/web/src/theme/MotionScheme.ts:2` defines `"standard" | "expressive"`; matches the spec scheme set.
- Token keys: `packages/web/src/theme/SpringEasing.ts:20-36` lists six keys per scheme: `fast-spatial`, `default-spatial`, `slow-spatial`, `fast-effects`, `default-effects`, `slow-effects`.
- Token mapping: those six keys map to `spatial.fast/default/slow` and `effects.fast/default/slow` in `m3e/spec/04-motion.md:7-12`; only ordering differs (m3e CSS uses size-family, spec config uses family.size). No semantic mismatch.
- Token emission: `packages/web/src/theme/ThemeElement.ts:323-324` builds `--md-sys-motion-spring-${name}` from those keys, confirming the `M3E-<SCHEME>-<FAMILY>-<SIZE>` decomposition.
- Compose names: `MotionScheme.kt:56-113` exposes the same six families: `default/fast/slow` × `Spatial/Effects`.
