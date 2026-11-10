# 6. Config

| Section | Field | Default | Validation |
|---|---|---|---|
| `[motion]` | scheme | expressive | standard or expressive |
| `[motion]` | reduce | false | bool |
| `[motion.spatial.<size>]` | damping, stiffness | scheme table | damping > 0, stiffness > 0 |
| `[motion.effects.<size>]` | damping, stiffness | scheme table | damping >= 1.0, stiffness > 0 |
| `[motion]` | preset | custom | smooth, balanced, playful, or custom |
| `[motion]` | ripple | true | bool |
| `[motion]` | panel | standard | standard, directional, depth, or fluid |
| `[theme]` | design_system | expressive | expressive or custom; anything else rejected |
| `[theme]` | density | custom | spacious, comfortable, compact, dense, or custom |
| `[theme]` | corner_style | material | material or fixed |
| `[shape]` | press_radii | squarer preset | non-negative radii tuple |
| `[shape]` | press_morph (P2) | unset | must be a ShapeName |
| `[widget.<name>]` | shape, shape_pressed, shape_hover (P2) | pill / inherits | existing ShapeName |
| `[widget.<name>]` | morph_spring | spatial.default | one of the six token strings |
| `[typography]` | font_variations (P3) | empty | valid OpenType tags |
| `[choreography]` | stagger_ms, hero_delay_ms (P3) | 50, 125 | >= 0 |

`design_system` is the fork's master preset switch. When expressive, the fork's expressive presets override `[shell.animation].speed`, `[shell.corner_radius_scale]`, and border booleans. When custom, every slider and boolean works as vanilla noctalia. No config keys are removed — the preset layer is additive.

Preset-to-slider interaction (DES-004):
- `[theme].density` writes `[accessibility].ui_scale`: spacious=1.30, comfortable=1.15, compact=1.00, dense=0.90. Dragging the ui_scale slider resets density to "custom".
- `[motion].preset` writes `[shell.animation].speed`: smooth=1.5, balanced=2.5, playful=3.5. Also selects SpringToken family in `MotionService::resolve` (smooth→effects.default, balanced→spatial.default, playful→spatial.fast). Dragging speed slider resets preset to "custom".
- All preset fields only shown in GUI when `design_system = "expressive"`. Under Classic (currently "custom"), presets are inert — sliders are source of truth.

Panel motion (DES-005):
- `[motion.panel]` selects the panel reveal animation variant. standard = current attached/detached reveal. depth = add uniform scale lerp(0.92, 1.0, progress) to panel chrome. directional = placement-aware slide from nearest edge for detached/floating panels (attached already enters from bar edge). fluid = spring decomposition on x/y + per-direction squash.

No keys removed. All default values preserve existing behavior.

Runtime widget state goes to `state.toml` via StateStore, never into config sections.
