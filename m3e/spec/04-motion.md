# 4. Motion tokens (Provisional until VG-08)

Springs are damping ratio, mass 1. Config keys use lowercase dot form; internal IDs use `M3E-<SCHEME>-<FAMILY>-<SIZE>`.

| Token (config) | Family | Expressive zeta/k | Standard zeta/k | Overshoot |
|---|---|---|---|---|
| spatial.fast | Spatial | 0.6 / 800 | 0.9 / 1400 | expressive yes, standard minimal |
| spatial.default | Spatial | 0.8 / 380 | 0.9 / 700 | same |
| spatial.slow | Spatial | 0.8 / 200 | 0.9 / 300 | same |
| effects.fast | Effects | 1.0 / 3800 | 1.0 / 3800 | none |
| effects.default | Effects | 1.0 / 1600 | 1.0 / 1600 | none |
| effects.slow | Effects | 1.0 / 800 | 1.0 / 800 | none |

| Animation class | Token family | Request |
|---|---|---|
| Position, size, radius, morph amount | spatial | requestFrameTick |
| Discrete shape switch | spatial (retarget) | requestLayout |
| Color, opacity, elevation | effects | requestRedraw |

## 4.5 Design system gating

| [theme].design_system | MotionService::resolve() behavior |
|---|---|
| expressive | Returns SpringToken table values directly, no speed multiplier. |
| custom | Returns SpringToken values × [shell.animation].speed² stiffness (existing behavior). |

*Note:* `[motion].scheme` and `[motion.reduce]` still apply in both modes — they're spec tokens and accessibility, not sliders.

## 4.6 Motion preset (DES-004)

| [motion].preset | Writes [shell.animation].speed | SpringToken family override |
|---|---|---|
| custom (default) | no change | none (per-token table from §4.1-4.4) |
| smooth | 1.5 | effects.default — no overshoot |
| balanced | 2.5 | spatial.default — subtle overshoot |
| playful | 3.5 | spatial.fast — max overshoot |

Preset active only when [theme].design_system = expressive AND [motion].preset != custom. Under design_system = classic, preset is inert; [shell.animation].speed is source of truth. Dragging speed slider resets preset to "custom".

## 4.7 Panel motion variants (DES-005)

| [motion].panel | Behavior (panel_manager.cpp reveal) |
|---|---|
| standard (default) | Current attached/detached progress spring. Clip + opacity only. |
| depth | Add uniform scale to panel chrome: scale = lerp(0.92, 1.0, progress) on entry, lerp(1.0, 0.92, 1-progress) on exit. Content scales with chrome. |
| directional | Placement-aware slide: attached panels already enter from bar edge (current). Detached/floating slide from nearest screen edge based on panel.position (top/bottom/left/right/center). Requires direction enum on panel + transform per direction. |
| fluid | Per-axis spring decomposition: x-translate and y-translate spring independently from reveal progress. Add per-direction squash (asymmetric scale on entry that settles to 1.0). Needs per-axis spring state per panel instance. |

Only shown in GUI when design_system = expressive. All variants respect [motion].reduce (snap-to-final, no animation). All variants respect [motion].preset for spring feel.
