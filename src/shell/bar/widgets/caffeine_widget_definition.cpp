#include "shell/bar/widgets/caffeine_widget_definition.h"

const vespertine::bar::WidgetDefinition<std::monostate>& caffeineWidgetDefinition() {
  static const vespertine::bar::WidgetDefinition<std::monostate> definition{
      .type = "caffeine",
  };
  return definition;
}
