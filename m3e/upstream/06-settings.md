# Upstream diff — settings

| File | Δ | Change |
|---|---|---|
| `src/shell/settings/settings_registry.cpp` | +51/-15 | Design-system dropdown, preset selectors, border-toggle gating, community/built-in template store buttons, corner_shape replacing slider |
| `src/shell/settings/settings_content.cpp` | +3 | Built-in template card summary row + modal wiring |
| `src/shell/settings/settings_sheet.cpp` | +25 | Sheet scroll/body handling |
| `src/shell/settings/settings_sidebar.cpp` | +24/-17 | Expanded padding, ghost buttons |
| `src/shell/settings/settings_window.{h,cpp}` | +3/+21 | New callbacks (openBuiltinTemplateStore etc.) |
| `src/shell/settings/settings_window_mutations.cpp` | +9 | Rebuild suppression for community_ids |
| `src/shell/settings/settings_window_palette.cpp` | +2 | Palette inspector |
| `src/shell/settings/settings_window_popups.cpp` | +32 | Community + built-in template-store modals |
| `src/shell/settings/settings_window_scene.cpp` | +23/-22 | Callback wiring, registry action patch |

rev8: the Community Template Store and Built-in Template Store both render as sheets (`SettingsSheetModal`) built from a shared `TemplateStoreTile` list with native search/category/sort, hover tooltips, and click routing. An inline-accordion attempt was reverted due to a crash.
