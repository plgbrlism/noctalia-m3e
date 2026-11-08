# 1. Verification gates

| ID | Gate | Method | If it fails |
|---|---|---|---|
| VG-00 | Baseline commit and component names | `git rev-parse HEAD`; inspect ARCHITECTURE.md, AnimationManager, GlesRenderBackend, rect program, CairoTextRenderer, config_types.* | Remap spec IDs |
| VG-01 | Spring syntax, retargeting, and damping convention (ratio vs absolute) | Read AnimationManager; unit harness | Add retargeting as P0; add conversion per MOT-SPRING-001 |
| VG-02 | Rect shader accepts extra uniforms without a new program variant; per-corner radii supported; uniform array vs R32F texture for r(theta) table | Scratch branch with dummy uniform | Re-evaluate SHP-EXT-001 and SHP-004 effort |
| VG-03 | Pango variable axes (wght/wdth/GRAD) | Pango attribute test | Typography descoped with warning |
| VG-04 | Controls consume M3 color roles | Audit Button, Label, panels vs `lib/ui/palette.h` | Color audit expands |
| VG-05 | ShapeController can live in `lib/ui/controls/` | Inspect control base classes | Use smaller control-level hook |
| VG-06 | Plugin surface has no renderer/animation hooks | Audit `luau_host.cpp`, `plugin_bindings.cpp` | Expected closed |
| VG-07 | Token and shape names vs m3e and Compose | Compare `ShapeName.ts`, `ThemeElement.ts`, `MotionScheme.kt` | Rename before M1 |
| VG-08 | Spring numbers vs Compose `MotionScheme.kt` | Diff against section 4 | Section 4 stays Provisional; springs load from one table so a fix is one line |
