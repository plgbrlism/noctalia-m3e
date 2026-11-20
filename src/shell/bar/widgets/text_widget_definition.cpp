#include "shell/bar/widgets/text_widget_definition.h"

const vespertine::bar::WidgetDefinition<TextWidget::Options>& textWidgetDefinition() {
  using vespertine::bar::field;
  using Options = TextWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "text",
      .fields = {
          field<&Options::text>({
              .key = "text",
          }),
      },
  };
  return definition;
}
