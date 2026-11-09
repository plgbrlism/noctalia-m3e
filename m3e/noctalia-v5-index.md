# noctalia-v5 index

Generated from ARCHITECTURE.md at commit ed7a4f167.
Regenerate only via the spec-verify or spec-amend skills.

## Animation core

- `lib/render/animation/animation_manager.*` — AnimationManager spring/tween core
- `lib/render/animation/motion_service.h` — motion service (spring tokens likely live here or adjacent)
- `lib/wayland/surface.cpp` — Surface::requestFrameTick() (line 1146); per-surface frame request
- `src/theme/theme_service.cpp` — AnimationManager::tick() call site (line 631)
- `src/shell/lockscreen/lockscreen_widgets_host.cpp` — per-widget AnimationManager, tick() at line 334
- `src/shell/desktop/desktop_widget.h` — widget-level requestFrameTick() wrapper (line 113)
- `lib/ui/controls/` — controls animate through `AnimationManager`
- Invariants: PHASE-001, MOT-SPRING-001, MOT-INT-001, MOT-DT-001, MOT-IDLE-001, MOT-REDUCE-001, LIB-001
- Work items: MOT-001, MOT-002, MOT-003, MOT-004, MOT-005, MOT-006, CHO-001, CHO-002
---
## Rect primitive

- `lib/render/programs/` — generic rect GLES program and primitive shaders
- `lib/render/core/` — neutral style/data structs and shape data headers
- Invariants: SHP-EXT-001, SHP-CLAMP-001, SHP-DAMAGE-001, SHP-SCALE-001
- Work items: SHP-001, SHP-002, SHP-003, SHP-004, SHP-005, CMP-003
---
## RenderCore

- `lib/render` — generic render library (rect, text, image, glyph, spinner, blur)
- `lib/render/animation/` — AnimationManager, MotionService, spring integration
- `render/core/` — RenderContext types, backend, texture, style headers
- Invariants: INV-01, SHP-EXT-001, SHP-SCALE-001
- Work items: SHP-002, SHP-003, SHP-005, CMP-003
---
## Controls

- `lib/ui/controls/` — Button, Label, Switch, Chip, and other primitive controls
- `src/shell/controls/` — shell-owned controls: MarkdownView, SearchPicker, ColorPicker
- Invariants: INV-02, PHASE-002, LIB-001
- Work items: MOT-004, SHP-001, SHP-006, CMP-001, CMP-002, COL-001
---
## Shell panels

- `src/shell/surface/shadow.*` — shared shadow logic for panels, bars, dock
- `src/shell/render/` — shell-only custom draw nodes and render extensions
- `src/shell/desktop/widgets/` — desktop widget tick/request call sites
- Invariants: INV-02, INV-03, INV-04
- Work items: MOT-004, COL-003, SHP-007
---
## Config schema

- `src/config/config_types.*` — schema structs and enums
- `src/config/config_overrides.cpp` — override persistence and settings.toml writes
- Invariants: INV-07, INV-09
- Work items: CFG-001, MOT-006, SHP-007
---
## Theme and palette

- `lib/ui/palette.h` — Palette, ColorRole, ColorSpec definitions
- `lib/ui/style.h` — shared style constants and spacing
- `src/theme/fixed_palette_json.cpp` — palette JSON format upgrades
- Invariants: none
- Work items: COL-001, COL-002
---
## Text rendering

- `lib/render/text/glyph_registry.cpp` — glyph name resolution and curated aliases
- `lib/ui/controls/` — Label engine and text vertical centering
- `<verify>` — CairoTextRenderer, Pango, and Cairo path not stated
- Invariants: INV-02
- Work items: TYP-001, TYP-002
---
## Scripting

- `src/scripting/luau_host.cpp` — Luau VM host and plugin entry callbacks
- `src/scripting/plugin_bindings.cpp` — plugin-facing API bindings
- Invariants: INV-08
- Work items: PLG-001
---
## Wayland and compositor

- `lib/wayland` — protocol transport, plain xdg-toplevel globals
- `src/shell/wayland/` — shell-only compositor extensions and layer shell
- Invariants: INV-01
- Work items: none
---
## Application lifecycle

- `src/app/application.*` — Application lifecycle, service init, reload callback registration
- `src/app/application_services.*` — Service accessors and startup wiring (`MotionService::instance()` setup at :558)
- `src/config/config_service.*` — Config load, validation, reload callback iteration (:630 register, :670/:715 iterate)
- Invariants: INV-07, INV-09
- Work items: CFG-001
---
## Services

- `<verify>` — idle, PipeWire, clipboard, brightness, weather paths not stated
- Invariants: none
- Work items: none
---
## Launcher

- `third_party/fzy` — vendored fuzzy scoring
- `src/shell/controls/` — SearchPicker control used by launcher
- `<verify>` — launcher core and provider dispatch path not stated
- Invariants: INV-06
- Work items: none
