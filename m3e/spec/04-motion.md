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
