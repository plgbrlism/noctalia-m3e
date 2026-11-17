# 8. Tests

Keep these even solo. Everything else is Opt.

| ID | Test | Pass |
|---|---|---|
| AT-MOT-01 | Step response, all effects tokens, both schemes | Overshoot = 0 |
| AT-MOT-02 | Mid-flight retarget | No snap |
| AT-MOT-03 | Standard vs expressive | Measurably different |
| AT-MOT-04 | Idle harness | Zero ticks after settle |
| AT-MOT-05 | Inject 200 ms stall | Stable spring |
| AT-MOT-06 | `reduce = true` | No spatial overshoot, no choreography delay |
| AT-SHP-01 | Force overshoot past half-side | Radius clamped; morph amount in `[0,1]` |
| AT-SHP-02 | Render at 1.25x and 1.5x | Radii correct |
| AT-SHP-03 | Morph with overshoot, incremental render | No clipping |
| AT-SHP-04 | Bar/panel frame time vs baseline | No regression over 5% |
| AT-CFG-01 | Invalid token | Clear error, no partial apply. Verified in CFG-001, not MOT-001. |
| AT-CFG-02 | No `[motion]` section | Expressive applied. Verified in CFG-001, not MOT-001. |
| AT-CFG-03 | Effects damping 1.2 | Accepted |
| AT-PHASE-01 | Debug assertions | No Render-phase mutation; correct request types |
| AT-INV-01 | Include-path audit | No `src/` includes in `lib/` |
| AT-INV-02 | Grep gate | No raw nodes in `src/shell/` |
| AT-PRESET-01 | `[shell].shape = "expressive"` | `Style::shapeRestRadii()` returns `{4,12,4,12}` at scale=1.0 |
| AT-PRESET-02 | `[shell].shape = "classic"` | `Style::shapeRestRadii()` returns uniform `Style::scaledRadiusMd()` |
| AT-PRESET-03 | `[motion].scheme = "expressive"` | `MotionService::resolve` returns token table values |
| AT-PRESET-04 | `[motion].scheme = "off"` | `MotionService::resolve` returns zeta=1.0, k=1e6 (snap) |
| AT-PRESET-05 | Legacy `[shell.animation].speed` in config | Config parse rejects with clear error (key removed) |
| AT-PRESET-06 | Legacy `[shell].corner_radius_scale` in config | Config parse rejects with clear error |
| AT-PRESET-07 | Border config key removed from schema | Config parse rejects `[shell].button_borders` |
| AT-DES-01 | `[theme].design_system = "expressive"` (default) | `Style::designSystem()` = Expressive; `shapeRestRadii()` = `{4,12,4,12}` at scale 1.0; `MotionService::resolve` ignores speed |
| AT-DES-02 | `[theme].design_system = "custom"` | `Style::designSystem()` = Custom; `shapeRestRadii()` uniform; `MotionService::resolve` applies speed² multiplier |
| AT-DES-03 | Invalid `design_system` value | Parse rejects; no partial apply |
| AT-DES-04 | Hot reload: change `design_system` | `Style::setDesignSystem` updates; relayout triggered; AT-SHP-01/02 pass in new mode |
| AT-DES-05 | Expressive + all border config = true | No borders rendered |
| AT-DES-06 | Custom + speed = 3.35 | Spring feel matches pre-DES-001 baseline |
| AT-DES-07 | `[theme].density = "spacious"` | `[accessibility].ui_scale` set to 1.30; density=compact after slider drag sets ui_scale manually |
| AT-DES-08 | `[theme].corner_style = "fixed"` | `Button::syncPressMorph` does not start spring; progress stays 0; radii static across press |
| AT-DES-09 | `[motion_preset].preset = "playful"` | `[shell.animation].speed` set to 3.5; `MotionService::resolve` uses spatial.fast tokens; preset=custom after manual speed drag |
| AT-DES-10 | `[motion_preset].preset = "smooth"` | `MotionService::resolve` returns effects.default (no overshoot regardless of speed slider) |
| AT-DES-11 | design_system=classic + `[theme].density` set | Preset inert; ui_scale unchanged from config value |
| AT-PANEL-01 | `[motion_preset].panel = "standard"` | Panel reveal matches current behavior (regression) |
| AT-PANEL-02 | `[motion_preset].panel = "depth"` | Panel scale animates 0.92→1.0 on entry, 1.0→0.92 on exit |
| AT-PANEL-04 | `[motion_preset].panel = "fluid"` + reduce=true | Panel snaps to final state, no animation, all variants |
| AT-PANEL-05 | `[motion_preset].panel = "fluid"` | Transform identity at p=1; non-identity at p=0. Down: offsetY<0, scaleX>1. Up: offsetY>0, scaleX>1. Right: offsetX<0, scaleY>1. Left: offsetX>0, scaleY>1. Reduce motion: identity at any progress. |
| AT-DES-12 | Invalid enum value for density/corner_style/preset/panel | Config parse rejects with clear error; no partial apply |

*Note:* AT-PRESET-05/06/07 verify removals — legacy config files fail fast with actionable errors, no silent ignore.

P2 adds: AT-SHP all-35 render, star-convex per name, AA thin features, cross-profile morph interruptibility, AT-CMP golden checks, AT-COL contrast checker.

## Added in rev8

| ID | Test | Coverage |
|---|---|---|
| AT-CTL-01 | `tests/checkbox_test.cpp` | M3 Checkbox state layer, indeterminate state, hover tracking |
| AT-CTL-02 | `tests/button_shape_test.cpp` | Press morph radii clamp at 1.25x/1.5x |
| AT-CTL-03 | `tests/button_row_group_test.cpp` | Row grouping corner shaping |
| AT-UI-01 | `tests/tooltip_border_test.cpp` | Tooltip border toggle |
| AT-UI-02 | `tests/tooltip_surface_test.cpp` | Tooltip surface elevation |
| AT-CFG-04 | `tests/des_presets_test.cpp` | density/corner presets, migration v22 |
| AT-CFG-05 | `tests/config_motion_test.cpp` | `[motion]` scheme/reduce/token parse |
| AT-DES-13 | `tests/design_system_test.cpp` | `[theme].expressive` bool (replaced design_system) |
| AT-PANEL-06 | `tests/panel_reveal_test.cpp` | 4 panel motion variants + reduce |
| AT-MOT-07 | `tests/spring_test.cpp`, `tests/idle_test.cpp` | Spring settle + zero idle ticks |
| AT-SLD-01 | `tests/slider_test.cpp` | Standard and Range Slider non-zero handle bounds, exact sliderGap spacing, endpoint clamping, press squish, and 1.25x/1.5x scaling |
| AT-SLD-02 | `tests/range_slider_test.cpp` | RangeSlider dual handle visibility, 4-boundary sliderGap spacing, low <= high ordering, and active track hiding on convergence |

Template store (community + built-in modal) verified manually: row 1 not clipped, hover tooltip on row body/checkbox, readme opens browser, checkbox toggles config, search/category/sort/empty state, no per-row tab stop.
