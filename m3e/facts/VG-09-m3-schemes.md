# VG-09 — M3 scheme currency and upstream feature parity
Verdict: partial
Baseline commit: ed7a4f167
Date: 2026-11-09

## Evidence
- `third_party/material_color_utilities/cpp/scheme/scheme_expressive.h` — SchemeExpressive vendored
- `src/theme/scheme.h:29-38` — Scheme enum lacks Expressive variant
- `src/theme/contrast.h:7-8` — contrastRatio and relativeLuminance helper symbols exist

## Impact
- COL-001: Contrast test-only implementation using contrast.h / palette helpers.
- COL-002: Exposing SchemeExpressive requires adding `Expressive` to `Scheme` enum in `src/theme/scheme.h` and dispatching in `src/theme/m3_schemes.cpp`.
