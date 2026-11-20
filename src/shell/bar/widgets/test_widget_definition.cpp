#include "shell/bar/widgets/test_widget_definition.h"

const vespertine::bar::WidgetDefinition<std::monostate>& testWidgetDefinition() {
  static const vespertine::bar::WidgetDefinition<std::monostate> definition{
      .type = "test",
  };
  return definition;
}
