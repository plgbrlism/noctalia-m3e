# Upstream diff — panel system

| File | Δ | Change |
|---|---|---|
| `src/shell/panel/panel_manager.cpp` | +55/-28 | Reveal variants (standard/depth/directional/fluid), reduce-motion snapping |
| `src/shell/panel/panel.h` | +6 | Variant state on panel |
| `src/shell/panel/panel_reveal_helpers.h` | +62 new | Shared reveal transform + squash math |
| `src/shell/panel/panel_button_style.h` | +19 | Ghost icon button helper |
| `src/shell/panel/panel_header_style.h` | +8 new | Tonal header surface |
| `src/shell/panel/panel_surface_style.h` | +6 | Surface style |
| `src/shell/panel/persistent_panel_host.cpp` | +6 | Variant propagation |

Upstream reveal was a single fixed animation. The fork selects one of four via `[motion_preset].panel` and keeps reduce-motion as an instant snap.

Work item: DES-005. Tests: `tests/panel_reveal_test.cpp`, AT-PANEL-01..06.
