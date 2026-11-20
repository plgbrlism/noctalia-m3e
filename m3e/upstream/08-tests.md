# Upstream diff — tests

New suites added by the fork (all `+` lines):

| File | Lines | Area |
|---|---|---|
| `tests/spring_test.cpp` | 391 | Spring settle, retarget, cancel, idle |
| `tests/config_motion_test.cpp` | 207 | `[motion]` parse/validation |
| `tests/motion_scheme_test.cpp` | 199 | Token tables, scheme mapping |
| `tests/idle_test.cpp` | 157 | Zero idle ticks |
| `tests/button_row_group_test.cpp` | 138 | Row grouping |
| `tests/button_shape_test.cpp` | 106 | Radii clamp, 1.25x/1.5x |
| `tests/panel_reveal_test.cpp` | 73 | Panel motion variants |
| `tests/tooltip_border_test.cpp` | 70 | Tooltip border toggle |
| `tests/des_presets_test.cpp` | 19 | Density/corner presets |
| `tests/design_system_test.cpp` | 35 | `[theme].expressive` |
| `tests/style_test.cpp` | 40 | Style tokens |
| `tests/tooltip_surface_test.cpp` | 20 | Tooltip elevation |

Modified: `tests/config_schema_roundtrip_test.cpp`, `tests/ui_tree_reconciler_test.cpp` (schema + widget updates).

Test IDs and acceptance criteria are in `m3e/spec/08-tests.md` (rev8 section). Run all with `ninja -C build-release test` / `meson test -C build-release`.

## Upstream sync 2026-11-20

| Commit | Area | Decision | Reason |
|---|---|---|---|
| `0a0d6380` | TEST_CHECK conversion | left out | 120 files, mechanical, low value |
| `0e32beee` | style props reset | adapt to garden | includes `ui_tree_reconciler` test |
| `6bc8abe9` | notification actions | adapt to garden | includes notification history test |
| `6fdf8858` | smart auto-hide | adapt to garden | feature test coverage |
| `a81e34e7` | bar smart layer option | adapt to garden | config schema test updates |
