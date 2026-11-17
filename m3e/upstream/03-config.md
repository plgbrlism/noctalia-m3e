# Upstream diff — config and app services

| File | Δ | Change |
|---|---|---|
| `src/config/config_types.h` | +60/-4 | `MotionConfig`, `MotionPresetConfig`, `ThemeConfig` explicit expressive/density/corner_style/corner_shape fields, wallpaper_scheme, migration v22 |
| `src/config/schema/config_schema.{h,cpp}` | +168/-3 | `[motion]`, `[motion_preset]`, `[theme]` preset enums and validators; fail-fast for removed keys |
| `src/config/config_overrides.cpp` | modified | Override handling for new sections |
| `src/config/config_migrations.cpp` | +147 | Migration v22 (density rework, remove legacy motion/design_system keys) |
| `src/app/application_services.cpp` | +23/-9 | `Style::setDesignSystem`, `setCornerStyle`, `setDensityScale`, `setCornerShape`, `MotionService::setMotionConfig` wiring |

rev8 config reality (see `m3e/spec/06-config.md`):
- `[theme].expressive` bool replaces `design_system`.
- `[theme].density` = comfortable/standard/compact.
- `[theme].corner_shape` replaces `corner_radius_scale`.
- `[motion_preset]` keeps only `panel`.
- `motion_preset.preset`, `ripple`, deprecated reduce/motion presets removed.

Work item: CFG-001, DES-004. Test: `AT-CFG-01..05`, `AT-DES-07..13`.
