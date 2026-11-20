# Upstream diff — theme and color

| File | Δ | Change |
|---|---|---|
| `src/theme/scheme.h` | +7 | `Scheme::Expressive` + `"m3-expressive"` key in `kSchemeEntries`, `isMaterialScheme()` |
| `src/theme/m3_schemes.cpp` | +3 | `SchemeExpressive` dispatch in `makeScheme()` |
| `assets/translations/en.json` | +45/-12 | Labels for design system, presets, density, corner shape, template store |

`SchemeExpressive` was already vendored in `third_party/material_color_utilities`; the fork only exposes it to config and settings (COL-002). See `m3e/facts/VG-09-m3-schemes.md` (verdict updated to pass).

Role usage follows `lib/ui/palette.h` ColorRole set. Note `Outline` is not a ColorRole in this codebase — only `OutlineVariant` exists; the M3 slider spec's "Outline" maps to `OutlineVariant`.

## Upstream sync 2026-11-20

| Commit | Area | Decision | Reason |
|---|---|---|---|
| `00aa20f8`, `f04c3207` | default palette tweaks | left out | M3E intentionally changed palette and role usage |
