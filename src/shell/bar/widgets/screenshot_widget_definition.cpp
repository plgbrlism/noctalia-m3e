#include "shell/bar/widgets/screenshot_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<ScreenshotWidget::Options>& screenshotWidgetDefinition() {
  using Options = ScreenshotWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "screenshot",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
  };
  return definition;
}
