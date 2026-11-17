# Upstream diff — UI controls and style

| File | Δ | Change |
|---|---|---|
| `lib/ui/controls/button.{h,cpp}` | +19/+170/-6 | Press-morph per-corner radii, asymmetric Material rest radii `{4,12,4,12}`, clamp + scale plumbing, `currentRadiiForTest`, spring-driven `syncPressMorph` |
| `lib/ui/controls/flex.{h,cpp}` | +?/+47 | `setRadii`, row-grouping participation helpers |
| `lib/ui/controls/segmented.{h,cpp}` | separators removed, tab/ghost palettes fixed | rev8 |
| `lib/ui/controls/stepper.{h,cpp}` | segmented stepper row layout | rev8 |
| `lib/ui/controls/toggle.cpp` | expressive halo hover, state layer, check icon | rev8 (M3 toggle) |
| `lib/ui/style.{h,cpp}` | +42/+98/-4 | `DesignSystem`, `CornerStyle`, `CornerShapePreset`, `Density`, `cornerShape()`, `densityScale()`, `designSystem()`, larger control heights; slider/toggle tokens rev8 |
| `src/shell/panel/panel_header_style.h` | new | Shared header surface helper (`applyHeaderSurface`) |
| `src/shell/panel/panel_button_style.h` | modified | 48/21px ghost icon buttons |

M3 control redesign in rev8: Checkbox with state layer + indeterminate, Toggle with halo hover and check icon, Slider XS standard/range with stop dots and asymmetric track corners, template-store rows using a shared `TemplateStoreTile`.

Invariants: INV-02 (controls own nodes), PHASE-002, SHP-CLAMP-001, SHP-SCALE-001.
