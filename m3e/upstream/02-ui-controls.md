# Upstream diff — UI controls and style

| File | Δ | Change |
|---|---|---|
| `lib/ui/controls/button.{h,cpp}` | +19/+170/-6 | Press-morph per-corner radii, asymmetric Material rest radii `{4,12,4,12}`, clamp + scale plumbing, `currentRadiiForTest`, spring-driven `syncPressMorph` |
| `lib/ui/controls/flex.{h,cpp}` | +?/+47 | `setRadii`, row-grouping participation helpers |
| `lib/ui/controls/segmented.{h,cpp}` | separators removed, tab/ghost palettes fixed | rev8 |
| `lib/ui/controls/stepper.{h,cpp}` | segmented stepper row layout | rev8 |
| `lib/ui/controls/toggle.{h,cpp}` | animated hover/press halo, centered check glyph, press squish | rev8 (M3 toggle) |
| `lib/ui/controls/slider.{h,cpp}` | M3E XS 7-Box layout, 6px gaps, press squish animation, stop dots, `setScale()` plumbing | rev8 |
| `lib/ui/controls/range_slider.{h,cpp}` | M3E XS 7-Box layout, 6px gaps, press squish animation, stop dots, `setScale()` plumbing | rev8 |
| `lib/ui/builders.cpp` | propagate `trackHeight`-derived `scale` to `Slider::setScale()` and `RangeSlider::setScale()` | rev8 |
| `lib/ui/style.{h,cpp}` | +42/+98/-4 | `DesignSystem`, `CornerStyle`, `CornerShapePreset`, `Density`, `cornerShape()`, `densityScale()`, `designSystem()`, larger control heights; slider/toggle tokens rev8 |
| `src/shell/panel/panel_header_style.h` | new | Shared header surface helper (`applyHeaderSurface`) |
| `src/shell/panel/panel_button_style.h` | modified | 48/21px ghost icon buttons |
| `lib/ui/controls/context_menu.cpp` | rev9 | Neutral hover uses OnSurface; destructive hover keeps Error at 16% |

M3 control redesign in rev8: Checkbox with state layer + indeterminate; Toggle with animated hover/press halo and centered check glyph; Slider and RangeSlider with M3E XS Box-owned 7-node layout (inactive tracks, active track, pill handles with press squish, stop dots, 6px gaps) and fractional scale propagation via `setScale()`; template-store rows using a shared `TemplateStoreTile`.

Invariants: INV-02 (controls own nodes), PHASE-002, SHP-CLAMP-001, SHP-SCALE-001.

## Upstream sync 2026-11-20

| Commit | Area | Decision | Reason |
|---|---|---|---|
| `2feffcaf` | live corner-radius rescale | left out | collides with SHP-001/002 and `corner_shape` |
| `7111f2f4` | hide close buttons live | left out | collides with M3E button and style redesign |
| `0e32beee` | style props reset | adapt to garden | small box/flex fix |
| `e56dee2c`, `23076844` | image and avatar snapping | adapt to garden | small render-image fixes |
| `bf887d5b` | GTK font family | adapt to garden | review style and application paths |
| `6bc8abe9` | notification actions | adapt to garden | feature commit |
