#include "shell/bar/widgets/wallpaper_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<WallpaperWidget::Options>& wallpaperWidgetDefinition() {
  using Options = WallpaperWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "wallpaper",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
