# Upstream diff — shell surfaces

Touched files (all modified unless noted):

`src/shell/bar/bar.cpp`, `src/shell/clipboard/clipboard_panel.cpp`, `src/shell/control_center/{control_center_panel.{h,cpp},tabs/*}`, `src/shell/desktop/desktop_widgets_host.cpp`, `src/shell/desktop/editor/desktop_widgets_editor.cpp`, `src/shell/dialogs/file_dialog_view.cpp`, `src/shell/dialog_popup_settings.h`, `src/shell/dock/{dock_instance.cpp,dock_items.cpp}`, `src/shell/greeter/greeter_appearance_sync.cpp`, `src/shell/launcher/launcher_panel.cpp`, `src/shell/lockscreen/lockscreen_widgets_host.cpp`, `src/shell/notification/notification_toast.cpp`, `src/shell/osd/{osd_overlay.{h,cpp}}`, `src/shell/plugin_layer/plugin_layer.cpp`, `src/shell/polkit/polkit_panel.cpp`, `src/shell/switcher/window_switcher.cpp`, `src/shell/tooltip/{tooltip_manager.{h,cpp}}`, `src/shell/wallpaper/panel/{wallpaper_panel.{h,cpp}}`, `src/system/telemetry_service.cpp`.

Themes:
- Tonal surface + typography applied to all panel headers (commit 1bfb7572e, `panel_header_style.h`).
- M3 tokens consumed via `MotionService::resolve` for bar/dock/tooltip animations.
- Tooltip border toggle and surface elevation (UI-001/UI-001b).
- Control-center sidebar layout and native scrolling fix; wallpaper/launcher category and filter polish.
- OSD and notification surfaces moved to `surface_container_high`.

Cross-reference: COL-003 (tonal elevation), DES-003 (border gating rev6), rev8 settings cleanup.
