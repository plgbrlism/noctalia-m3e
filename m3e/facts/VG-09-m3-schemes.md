# VG-09 — M3 scheme currency
Verdict: pass (expressive vendored and exposed; 2025-spec vendor refresh remains a future item)
Baseline commit: ed7a4f167
Amended: 2026-11-17 (post COL-002, commit d998410c0-era)

## Evidence
- SchemeExpressive vendored — third_party/material_color_utilities/cpp/scheme/scheme_expressive.cc (contains kHues, kSecondaryRotations, kTertiaryRotations)
- Exposed — `src/theme/scheme.h` has `Scheme::Expressive` and the `"m3-expressive"` key in `kSchemeEntries` (line 35); `isMaterialScheme()` includes it (line 58)
- Dispatched — `src/theme/m3_schemes.cpp` includes `scheme_expressive.h` and `makeScheme()` maps `Scheme::Expressive` to `SchemeExpressive`
- Contrast helpers present — src/theme/contrast.h (relativeLuminance, contrastRatio)
- Vendored copy is 2023-era (copyright Google LLC 2023); 2025-spec refresh is out of scope for M3

## Resolution
- COL-001 = test-only against src/theme/contrast.h. No palette.h additions.
- COL-002 = enum + dispatch + config + settings dropdown. No vendor changes. Shipped: `wallpaper_scheme = "m3-expressive"` accepted.
- 2025-spec refresh = future work item, not tracked in M3.
