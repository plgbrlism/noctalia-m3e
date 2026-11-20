#include "shell/bar/widgets/session_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<SessionWidget::Options>& sessionWidgetDefinition() {
  using Options = SessionWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "session",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
