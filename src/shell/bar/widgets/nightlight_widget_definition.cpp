#include "shell/bar/widgets/nightlight_widget_definition.h"

const vespertine::bar::WidgetDefinition<std::monostate>& nightlightWidgetDefinition() {
  static const vespertine::bar::WidgetDefinition<std::monostate> definition{
      .type = "nightlight",
  };
  return definition;
}
