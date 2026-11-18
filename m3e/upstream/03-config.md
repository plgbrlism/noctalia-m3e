# Upstream diff — config and app services

| File | Δ | Change |
|---|---|---|
| `src/config/config_types.h` | refactor | `MotionUiConfig{expressive,panel}`, `AccessibilityConfig::density`, `ShellConfig::cornerStyle/cornerShape`; old `MotionConfig`/`MotionPresetConfig` and `ThemeConfig` UI keys removed |
| `src/config/schema/config_schema.{h,cpp}` | refactor | `[motion]`, `[accessibility]`, `[shell]` schemas; spring-token and `motionPresetSchema()` removed |
| `src/config/config_overrides.cpp` | modified | `configEqual` and `computeConfigChangeSet` use single motion struct |
| `src/config/config_migrations.cpp` | +v24 | Relocates UI keys into shell/accessibility/motion; v20 message targets shell.corner_shape |
| `src/app/application_services.cpp` | modified | `Style::setDesignSystem`, `setCornerStyle`, `setDensityScale`, `setCornerShape` from new sections; `MotionService::setMotionConfig` dropped |

Current config reality:
- `[motion].expressive` bool replaces the old `[theme].expressive` (Button Morphing toggle).
- `[motion].panel` = standard/directional/depth/fluid; default directional.
- `[accessibility].density` = comfortable/standard/compact.
- `[shell].corner_style` = material/fixed; `[shell].corner_shape` = sharp/standard/round.
- v24 moves all five keys and removes old spring/motion_preset config. Internal fixed spring tables remain.

Work item: CFG-REFACTOR. Test: config_motion/config_migration/config_schema_roundtrip/config_path_resolution.
