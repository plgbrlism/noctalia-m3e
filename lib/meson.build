# vespertine-ui: UI controls, renderer, Wayland transport, and core utilities
# shared by Vespertine applications. lib/ is its include root; it never sees src/.
#
# Self-contained: a consumer project (C++23, Meson >= 1.1) runs `subdir('lib')`
# and reads no other file outside lib/. Optional input, set before subdir():
#   vespertine_ui_build_codecs  true: enable the codec pack (default: false)
#   vespertine_ui_system_wuffs  true: compile the enabled codec pack against a system
#                             wuffs-v0.4.c instead of lib/third_party/wuffs
#
# Outputs:
#   vespertine_ui_dep              core library: render, text, scene/input, Wayland
#                                transport, controls (incl. Image), i18n service;
#                                system dependencies only
#   vespertine_ui_codecs_dep       only when enabled; opt-in codec pack: Wuffs, ICO, WebP, JPEG XL,
#                                SVG (librsvg), runtime-loaded libheif, image
#                                encoders, thumbnails; install it with
#                                codecs::installImageDecoder()
#   vespertine_ui_lib              core static library
#   vespertine_ui_codecs_lib       codec static library, only when enabled
#   vespertine_ui_protocol_headers              generated client protocol headers
#   vespertine_ui_assets_dir       assets the library loads (glyph font); install
#                                its contents into the application's asset dir
#
# Variables defined here use the vespertine_ui_ / _vespertine_ui_ prefixes, since a
# subdir shares the consumer's variable namespace.

_vespertine_ui_cc = meson.get_compiler('c')

# ── Dependencies ──────────────────────────────────────────────────────────────
_vespertine_ui_egl_dep = dependency('egl', required: false)
if not _vespertine_ui_egl_dep.found()
  _vespertine_ui_egl_dep = dependency('wayland-egl', required: false)
endif
if not _vespertine_ui_egl_dep.found()
  _vespertine_ui_egl_dep = dependency('epoxy')
endif
_vespertine_ui_gles2_dep = dependency('glesv2', required: false)
if not _vespertine_ui_gles2_dep.found()
  _vespertine_ui_gles2_dep = dependency('epoxy')
endif

if not _vespertine_ui_cc.has_header('stb/stb_image_resize2.h')
  error('vespertine-ui: stb/stb_image_resize2.h not found')
endif

_vespertine_ui_system_dependencies = [
  dependency('wayland-client'),
  _vespertine_ui_egl_dep,
  _vespertine_ui_gles2_dep,
  dependency('wayland-egl'),
  dependency('freetype2'),
  dependency('fontconfig'),
  dependency('cairo'),
  dependency('cairo-ft'),
  dependency('pango', version: '>=1.50'),
  dependency('pangocairo'),
  dependency('pangoft2'),
  dependency('harfbuzz'),
  dependency('xkbcommon'),
  dependency('glib-2.0'),
  dependency('nlohmann_json'),
]

# ── Compile arguments ─────────────────────────────────────────────────────────
_vespertine_ui_cpp_args = [
  '-Wall', '-Wextra', '-Wno-missing-field-initializers', '-Wpedantic', '-Wconversion', '-Wshadow',
  '-fno-strict-aliasing', '-fno-strict-overflow',
  '-DVESPERTINE_UI_INSTALL_PREFIX="' + get_option('prefix') + '"',
  '-DVESPERTINE_UI_INSTALL_DATADIR="' + get_option('datadir') + '"',
]

# ── Wayland protocols bound by the library ────────────────────────────────────
_vespertine_ui_wayland_scanner = find_program('wayland-scanner')
_vespertine_ui_protos_dir = dependency('wayland-protocols').get_variable('pkgdatadir')

_vespertine_ui_protocol_sources = []
vespertine_ui_protocol_headers = []

foreach _vespertine_ui_p : [
  ['xdg-output-unstable-v1',
    _vespertine_ui_protos_dir / 'unstable/xdg-output/xdg-output-unstable-v1.xml'],
  ['xdg-shell',
    _vespertine_ui_protos_dir / 'stable/xdg-shell/xdg-shell.xml'],
  ['xdg-activation-v1',
    _vespertine_ui_protos_dir / 'staging/xdg-activation/xdg-activation-v1.xml'],
  ['fractional-scale-v1',
    _vespertine_ui_protos_dir / 'staging/fractional-scale/fractional-scale-v1.xml'],
  ['viewporter',
    _vespertine_ui_protos_dir / 'stable/viewporter/viewporter.xml'],
  ['text-input-unstable-v3', files('protocols/text-input-unstable-v3.xml')],
  ['cursor-shape-v1', files('protocols/cursor-shape-v1.xml')],
]
  _vespertine_ui_h = custom_target(_vespertine_ui_p[0] + '-client-header',
    input:   _vespertine_ui_p[1],
    output:  _vespertine_ui_p[0] + '-client-protocol.h',
    command: [_vespertine_ui_wayland_scanner, 'client-header', '@INPUT@', '@OUTPUT@'],
  )
  _vespertine_ui_c = custom_target(_vespertine_ui_p[0] + '-client-source',
    input:   _vespertine_ui_p[1],
    output:  _vespertine_ui_p[0] + '-client-protocol.c',
    command: [_vespertine_ui_wayland_scanner, 'private-code', '@INPUT@', '@OUTPUT@'],
    depends: _vespertine_ui_h,  # ensure .h exists before this .c is compiled (it #includes it)
  )
  _vespertine_ui_protocol_sources += _vespertine_ui_c
  vespertine_ui_protocol_headers += _vespertine_ui_h
endforeach

# ── Core library ──────────────────────────────────────────────────────────────
_vespertine_ui_sources = files(
  'core/application_identity.cpp',
  'core/deferred_call.cpp',
  'core/files/resource_paths.cpp',
  'core/inotify/inotify.cpp',
  'core/input/key_chord.cpp',
  'core/input/keybind_matcher.cpp',
  'core/log.cpp',
  'core/process/process_fds.cpp',
  'core/timer_manager.cpp',
  'core/ui_phase.cpp',
  'core/ui_strings.cpp',
  'i18n/i18n.cpp',
  'i18n/i18n_service.cpp',
  'i18n/language_tag.cpp',
  'render/animation/animation.cpp',
  'render/animation/animation_manager.cpp',
  'render/animation/motion_scheme.cpp',
  'render/animation/motion_service.cpp',
  'render/backend/gles_framebuffer.cpp',
  'render/backend/gles_render_backend.cpp',
  'render/backend/gles_texture_manager.cpp',
  'render/backend/render_backend.cpp',
  'render/core/async_texture_cache.cpp',
  'render/core/color.cpp',
  'render/core/image_decoder.cpp',
  'render/core/image_orientation.cpp',
  'render/core/image_resize.cpp',
  'render/core/shader_program.cpp',
  'render/core/shared_texture_cache.cpp',
  'render/core/stb_image_resize_impl.cpp',
  'render/gl_shared_context.cpp',
  'render/programs/blur_program.cpp',
  'render/programs/glyph_program.cpp',
  'render/programs/image_program.cpp',
  'render/programs/rect_program.cpp',
  'render/programs/ring_program.cpp',
  'render/programs/spinner_program.cpp',
  'render/render_context.cpp',
  'render/render_target.cpp',
  'render/scene/input_area.cpp',
  'render/scene/input_dispatcher.cpp',
  'render/scene/node.cpp',
  'render/scene/ring_node.cpp',
  'render/text/cairo_glyph_renderer.cpp',
  'render/text/cairo_text_renderer.cpp',
  'render/text/font_registry.cpp',
  'render/text/font_weight_catalog.cpp',
  'render/text/glyph_registry.cpp',
  'ui/app_icon_colorization.cpp',
  'ui/builders.cpp',
  'ui/controls/box.cpp',
  'ui/controls/button.cpp',
  'ui/controls/checkbox.cpp',
  'ui/controls/collapsible.cpp',
  'ui/controls/color_swatch_preview.cpp',
  'ui/controls/context_menu.cpp',
  'ui/controls/context_menu_popup.cpp',
  'ui/controls/drag_source.cpp',
  'ui/controls/drop_zone.cpp',
  'ui/controls/fill_label.cpp',
  'ui/controls/flex.cpp',
  'ui/controls/glyph.cpp',
  'ui/controls/grid_tile.cpp',
  'ui/controls/grid_view.cpp',
  'ui/controls/image.cpp',
  'ui/controls/input.cpp',
  'ui/controls/keybind_recorder.cpp',
  'ui/controls/label.cpp',
  'ui/controls/list_drag.cpp',
  'ui/controls/list_editor.cpp',
  'ui/controls/progress_bar.cpp',
  'ui/controls/ring.cpp',
  'ui/controls/radio_button.cpp',
  'ui/controls/range_slider.cpp',
  'ui/controls/roving_list_nav.cpp',
  'ui/controls/scroll_view.cpp',
  'ui/controls/scrollbar.cpp',
  'ui/controls/segmented.cpp',
  'ui/controls/select.cpp',
  'ui/controls/select_dropdown_popup.cpp',
  'ui/controls/separator.cpp',
  'ui/controls/slider.cpp',
  'ui/controls/spacer.cpp',
  'ui/controls/spinner.cpp',
  'ui/controls/stepper.cpp',
  'ui/controls/toggle.cpp',
  'ui/controls/virtual_grid_view.cpp',
  'ui/controls/virtual_list_view.cpp',
  'ui/drag_drop_controller.cpp',
  'ui/palette.cpp',
  'ui/popup_chrome.cpp',
  'ui/scroll_into_view.cpp',
  'ui/split_pane_focus.cpp',
  'ui/style.cpp',
  'ui/surface_shadow.cpp',
  'util/clamp.cpp',
  'util/string_utils.cpp',
  'wayland/popup_surface.cpp',
  'wayland/subsurface.cpp',
  'wayland/surface.cpp',
  'wayland/text_input_service.cpp',
  'wayland/toplevel_surface.cpp',
  'wayland/wayland_connection.cpp',
  'wayland/wayland_seat.cpp',
)

if get_option('b_sanitize') != 'none'
  _vespertine_ui_sources += files('core/lsan_suppressions.cpp')
endif

# '.' adds both lib/ and its build directory, where the protocol headers above
# are generated.
_vespertine_ui_inc = include_directories('.')

vespertine_ui_lib = static_library('vespertine-ui',
  sources: _vespertine_ui_sources + _vespertine_ui_protocol_sources + vespertine_ui_protocol_headers,
  dependencies: _vespertine_ui_system_dependencies,
  include_directories: _vespertine_ui_inc,
  cpp_args: _vespertine_ui_cpp_args,
)

vespertine_ui_dep = declare_dependency(
  link_with: vespertine_ui_lib,
  dependencies: _vespertine_ui_system_dependencies,
  include_directories: _vespertine_ui_inc,
  sources: vespertine_ui_protocol_headers,
)

vespertine_ui_assets_dir = meson.current_source_dir() / 'assets'

# ── Codec pack ────────────────────────────────────────────────────────────────
if get_variable('vespertine_ui_build_codecs', false)
  if is_variable('vespertine_ui_system_wuffs') and vespertine_ui_system_wuffs
    if not _vespertine_ui_cc.has_header('wuffs-v0.4.c')
      error('vespertine-ui: vespertine_ui_system_wuffs is set but wuffs-v0.4.c was not found')
    endif
    _vespertine_ui_wuffs_inc = []
  else
    _vespertine_ui_wuffs_inc = [include_directories('third_party/wuffs', is_system: true)]
  endif

  if not _vespertine_ui_cc.has_header('stb/stb_image_write.h')
    error('vespertine-ui: stb/stb_image_write.h not found')
  endif

  _vespertine_ui_dl_dep = meson.get_compiler('cpp').find_library('dl', required: false)

  _vespertine_ui_codecs_system_dependencies = [
    dependency('librsvg-2.0'),
    dependency('libwebp'),
    dependency('libwebpdemux'),
    dependency('libjxl'),
    dependency('libjxl_threads'),
  ] + (_vespertine_ui_dl_dep.found() ? [_vespertine_ui_dl_dep] : [])

  vespertine_ui_codecs_lib = static_library('vespertine-ui-codecs',
    sources: files(
      'render/codecs/codecs.cpp',
      'render/codecs/heif_library.cpp',
      'render/codecs/icon_color_scheme.cpp',
      'render/codecs/image_encoder.cpp',
      'render/codecs/image_file_loader.cpp',
      'render/codecs/raster_decoder.cpp',
      'render/codecs/svg_rasterizer.cpp',
      'render/codecs/thumbnail_service.cpp',
    ),
    dependencies: _vespertine_ui_codecs_system_dependencies + [vespertine_ui_dep],
    include_directories: _vespertine_ui_wuffs_inc,
    cpp_args: _vespertine_ui_cpp_args,
  )

  vespertine_ui_codecs_dep = declare_dependency(
    link_with: vespertine_ui_codecs_lib,
    dependencies: _vespertine_ui_codecs_system_dependencies + [vespertine_ui_dep],
  )
endif
