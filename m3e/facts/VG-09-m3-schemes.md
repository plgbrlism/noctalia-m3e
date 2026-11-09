# VG-09 — M3 scheme currency
Verdict: partial (expressive vendored, unexposed; exposure is trivial, 2025-spec vendor refresh is a future item)
Baseline commit: e862faaa6a95211216ec47931bcecd359e6cce99
Date: 2026-11-09

## Evidence
- SchemeExpressive vendored — third_party/material_color_utilities/cpp/scheme/scheme_expressive.cc (contains kHues, kSecondaryRotations, kTertiaryRotations)
- Not exposed — src/theme/scheme.h has no Expressive enum value; src/theme/m3_schemes.cpp has no dispatch case
- Contrast helpers present — src/theme/contrast.h (relativeLuminance, contrastRatio)
- Vendored copy is 2023-era (copyright Google LLC 2023); 2025-spec refresh is out of scope for M3

## Resolution
- COL-001 = test-only against src/theme/contrast.h. No palette.h additions.
- COL-002 = enum + dispatch + config + settings dropdown. No vendor changes.
- 2025-spec refresh = future work item, not tracked in M3.
