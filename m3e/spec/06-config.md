# 6. Config

| Section | Field | Default | Validation |
|---|---|---|---|
| `[motion]` | scheme | expressive | standard or expressive |
| `[motion]` | reduce | false | bool |
| `[motion.spatial.<size>]` | damping, stiffness | scheme table | damping > 0, stiffness > 0 |
| `[motion.effects.<size>]` | damping, stiffness | scheme table | damping >= 1.0, stiffness > 0 |
| `[shape]` | press_radii | squarer preset | non-negative radii tuple |
| `[shape]` | press_morph (P2) | unset | must be a ShapeName |
| `[widget.<name>]` | shape, shape_pressed, shape_hover (P2) | pill / inherits | existing ShapeName |
| `[widget.<name>]` | morph_spring | spatial.default | one of the six token strings |
| `[typography]` | font_variations (P3) | empty | valid OpenType tags |
| `[choreography]` | stagger_ms, hero_delay_ms (P3) | 50, 125 | >= 0 |

Runtime widget state goes to `state.toml` via StateStore, never into config sections.
