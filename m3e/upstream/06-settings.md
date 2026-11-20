# Upstream diff — settings

| File | Δ | Change |
|---|---|---|
| `src/shell/settings/settings_registry.cpp` | modified | UI paths moved to shell/accessibility/motion; motion group reordered and visibility-gated |
| `src/shell/settings/settings_content.cpp` | +3 | Built-in template card summary row + modal wiring |
| `src/shell/settings/settings_sheet.cpp` | +25 | Sheet scroll/body handling |
| `src/shell/settings/settings_sidebar.cpp` | +24/-17 | Expanded padding, ghost buttons |
| `src/shell/settings/settings_window.{h,cpp}` | +3/+21 | New callbacks (openBuiltinTemplateStore etc.) |
| `src/shell/settings/settings_window_mutations.cpp` | +9 | Rebuild suppression for community_ids |
| `src/shell/settings/settings_window_palette.cpp` | +2 | Palette inspector |
| `src/shell/settings/settings_window_popups.cpp` | +32 | Community + built-in template-store modals |
| `src/shell/settings/settings_window_scene.cpp` | +23/-22 | Callback wiring, registry action patch |
| `src/shell/settings/settings_window_popups.cpp` | rev9 | Search picker hosted in SettingsModalHost with standard header |
| `src/shell/settings/widget_add_popup.cpp` | rev9 | Shared header surface and larger picker/create sizing |
| `src/shell/settings/settings_sheet.cpp` | rev9 | Destructive sheet action uses Error background and OnError glyph |

Current settings paths:
- Corner Style → `{shell,corner_style}`; Corner Shape → `{shell,corner_shape}`.
- Density → `{accessibility,density}`.
- Panel Motion → `{motion,panel}`; Button Morphing → `{motion,expressive}`.
- Motion group order: Animations → Panel Motion → Animation Speed → Button Morphing.
- Last three hidden when `shell.animation.enabled` is false.

Community Template Store and Built-in Template Store both render as sheets (`SettingsSheetModal`) built from a shared `TemplateStoreTile` list with native search/category/sort, hover tooltips, and click routing.

## Upstream sync 2026-11-20

| Commit | Area | Decision | Reason |
|---|---|---|---|
| `2f3e3c4f` | settings registry split | left out | collides with M3E settings paths and visibility gating |
| `02cafcdf` | plugin store thumbnails | adapt to garden | small plugin store fix |
| `a81e34e7` | bar smart layer option | adapt to garden | settings path additions |
