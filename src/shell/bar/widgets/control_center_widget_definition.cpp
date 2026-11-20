#include "shell/bar/widgets/control_center_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<ControlCenterWidget::Options>& controlCenterWidgetDefinition() {
  using Options = ControlCenterWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "control-center",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
