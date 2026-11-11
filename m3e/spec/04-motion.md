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

| [motion_preset].preset | Writes [shell.animation].speed | SpringToken family override |
|---|---|---|
| custom (default) | no change | none (per-token table from §4.1-4.4) |
| smooth | 1.5 | effects.default — no overshoot |
| balanced | 2.5 | spatial.default — subtle overshoot |
| playful | 3.5 | spatial.fast — max overshoot |

Preset active only when [theme].design_system = expressive AND [motion_preset].preset != custom. Under design_system = classic, preset is inert; [shell.animation].speed is source of truth. Dragging speed slider resets preset to "custom".

## 4.7 Panel motion variants (DES-005)

| [motion_preset].panel | Behavior (panel_manager.cpp reveal) |
|---|---|
| standard (default) | Current attached/detached progress spring. Clip + opacity only. |
| depth | Add uniform scale to panel chrome: scale = lerp(0.92, 1.0, progress) on entry, lerp(1.0, 0.92, 1-progress) on exit. Content scales with chrome. |
| fluid | Slide + squash, single spring drives both. Detached/floating panels slide in from anchored direction (kFluidTravel=24px) with perpendicular squash (kFluidSquash=0.02) that settles to identity at p=1. Attached panels apply squash only (existing slide already directional). Per-axis spring decomposition (x and y settle on different timings) is deferred to a future "Fluid v2" item. Respects [motion].reduce — snaps to identity, no offset, no scale. |

Only shown in GUI when design_system = expressive. All variants respect [motion].reduce (snap-to-final, no animation). All variants respect [motion_preset].preset for spring feel.

## 4.8 Future: Fluid-without-clip-wipe (deferred)

Current Fluid behavior layers on top of noctalia's native directional clip-wipe (m_detachedRevealDirection drives clip expansion from the anchored edge for all variants). Fluid adds slide offset + perpendicular squash on top, so a top-anchored panel's reveal compounds into clip-grow-down + content-slide-down + wide-swell. This is coherent motion (both components travel the same direction) and ships as-is.

Deferred variant: "Fluid pure-slide" would gate the native clip-wipe off when variant == Fluid, leaving only the content offset + squash to drive the reveal. Content would appear to slide in from the edge with no growing window. Estimated ~30 line change to applyDetachedReveal + applyAttachedReveal:
- Set m_detachedRevealClipNode frame size to full surface immediately (no growth with progress) when variant == Fluid
- Let t.offsetX/Y alone animate reveal
- Same squash logic unchanged
- Attached panels: unclear whether to also gate clip (attached has no clip-wipe — it uses content translate already), so likely no-op

Not implemented — current layered behavior is coherent and preferred. Trigger to revisit: if users report "double motion" feel on large panels, or if a "pure slide" preset is requested as a distinct variant alongside Fluid.
