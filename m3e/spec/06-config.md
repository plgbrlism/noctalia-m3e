# 6. Config

Current schema as of rev8 (verified against `src/config/config_types.h` and `src/config/schema/config_schema.cpp`).

| Section | Field | Default | Validation |
|---|---|---|---|
| `[motion]` | scheme | expressive | standard or expressive |
| `[motion]` | reduce | false | bool |
| `[motion.spatial.<size>]` | damping, stiffness | scheme table | damping > 0, stiffness > 0 |
| `[motion.effects.<size>]` | damping, stiffness | scheme table | damping >= 1.0, stiffness > 0 |
| `[motion_preset]` | panel | standard | standard, directional, depth, or fluid |
| `[theme]` | expressive | true | bool; master expressive switch (replaces `design_system`) |
| `[theme]` | density | comfortable | comfortable, standard, or compact |
| `[theme]` | corner_style | material | material or fixed |
| `[theme]` | corner_shape | standard | sharp, standard, or round |
| `[theme]` | wallpaper_scheme | m3-content | any key in kSchemeEntries (incl. m3-expressive) |
| `[shape]` | press_radii | squarer preset | non-negative radii tuple |
| `[shape]` | press_morph (P2) | unset | must be a ShapeName |
| `[widget.<name>]` | shape, shape_pressed, shape_hover (P2) | pill / inherits | existing ShapeName |
| `[widget.<name>]` | morph_spring | spatial.default | one of the six token strings |
| `[typography]` | font_variations (P3) | empty | valid OpenType tags |
| `[choreography]` | stagger_ms, hero_delay_ms (P3) | 50, 125 | >= 0 |

`[theme].expressive` is the fork's master switch. When true, expressive presets override `[shell.animation].speed`, corner geometry, and border booleans; when false, vanilla sliders are the source of truth. `theme.corner_shape` replaces the removed `corner_radius_scale` slider (rev8).

`[theme].density` writes `[accessibility].ui_scale`: comfortable=1.15, standard=1.00, compact=0.90 (migration v22 reworked presets; the earlier spacious=1.30/dense=0.90 values are gone). Dragging the ui_scale slider resets density to a custom state.

Panel motion (`[motion_preset].panel`) selects the reveal variant: standard = upstream behavior, depth = uniform scale lerp, directional = placement-aware slide, fluid = spring decomposition with per-axis squash.

Removed keys (rev8, migration v22): `design_system`, `motion_preset.preset`, `motion_preset.ripple`, `[shell].corner_radius_scale`, deprecated motion presets and reduce/ripple toggles. Legacy configs fail fast with actionable errors.

Runtime widget state goes to `state.toml` via StateStore, never into config sections.
