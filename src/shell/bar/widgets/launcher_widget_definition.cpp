#include "shell/bar/widgets/launcher_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<LauncherWidget::Options>& launcherWidgetDefinition() {
  using Options = LauncherWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "launcher",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
