#include "shell/bar/widgets/clipboard_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"

const vespertine::bar::WidgetDefinition<ClipboardWidget::Options>& clipboardWidgetDefinition() {
  using Options = ClipboardWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "clipboard",
      .fields = vespertine::bar::glyphButtonFields<Options>(),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
