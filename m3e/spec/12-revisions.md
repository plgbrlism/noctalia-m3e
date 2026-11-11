# 12. Revision log

| Rev | Change |
|---|---|
| rev1 to rev3 | Initial spec, architecture alignment, m3e alignment |
| rev4 | Standard springs, idle/dt/reduce rules, clamp/AA/damage/scale rules, components, expanded tests |
| rev5 | Single shape model (radii first, r(theta) later, SHP-007 neutral-polygon removed); web easing (`linear()`, `cubic-bezier()`, AT-MOT-04 sampler) removed; spring convention gate added; press morph defaults to radii; amount clamp and settle epsilons added; duplicate rows merged; wavy progress gets a render-primitive note; MVP tier and timeline added; pipe-corrupted rows fixed; process-heavy tests moved to Opt |
| rev6 | Additive Design System coordinator. `[theme].design_system = expressive|custom` (default expressive). Master switch bypasses sliders when expressive (MotionService speed multiplier, corner_radius_scale, border booleans). When custom, all existing knobs work as before. Zero config removals — preset layer over existing config, not replacement. Tracks DES-001..003. |
| rev7 | DES-004 preset layer + DES-005 panel motion variants. Add `[theme].density`, `[theme].corner_style`, `[motion_preset].preset`, `[motion_preset].ripple`, `[motion_preset].panel`. All preset fields visible only under design_system=expressive; presets write ui_scale/speed and reset to "custom" on manual slider drag. Panel motion 4-variant enum (standard/depth/directional/fluid). Relabel design_system Custom→"Classic" in English UI (enum stays Custom). No config removals. Tracks DES-004 (M), DES-005 (L). |
