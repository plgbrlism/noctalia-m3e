#include "shell/bar/widgets/weather_widget_definition.h"

#include "shell/bar/glyph_value_definition.h"

const vespertine::bar::WidgetDefinition<WeatherWidget::Options>& weatherWidgetDefinition() {
  using vespertine::bar::field;
  using vespertine::bar::glyphPositionField;
  using vespertine::bar::peekDelayField;
  using vespertine::bar::peekField;
  using Options = WeatherWidget::Options;

  settings::WidgetSettingVisibility textShown;
  textShown.any = {
      {.key = "show_condition", .values = {"true"}},
      {.key = "show_temperature", .values = {"true"}},
  };

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "weather",
      .fields = {
          field<&Options::maxWidth>({
              .key = "max_length",
              .minValue = 40.0,
              .maxValue = 800.0,
              .step = 1.0,
          }),
          field<&Options::showCondition>({
              .key = "show_condition",
          }),
          field<&Options::showTemperature>({
              .key = "show_temperature",
          }),
          glyphPositionField<Options>(textShown),
          peekField<Options>(textShown),
          peekDelayField<Options>(textShown),
      },
  };
  return definition;
}
