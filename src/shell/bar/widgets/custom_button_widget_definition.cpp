#include "shell/bar/widgets/custom_button_widget_definition.h"

#include "shell/bar/glyph_button_definition.h"
#include "shell/bar/glyph_value_definition.h"

const vespertine::bar::WidgetDefinition<CustomButtonWidget::Options>& customButtonWidgetDefinition() {
  using vespertine::bar::field;
  using Options = CustomButtonWidget::Options;
  using vespertine::bar::glyphPositionField;
  using vespertine::bar::peekDelayField;
  using vespertine::bar::peekField;

  // The label is the value; the glyph slot is the glyph or the custom image.
  settings::WidgetSettingVisibility iconAndLabel;
  iconAndLabel.all = {{.key = "label", .nonEmpty = true}};
  iconAndLabel.any = {
      {.key = "glyph", .nonEmpty = true},
      {.key = "custom_image", .nonEmpty = true},
  };

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "custom_button",
      .fields = vespertine::bar::glyphButtonFields<Options>(
          field<&Options::label>({
              .key = "label",
          }),
          glyphPositionField<Options>(iconAndLabel), peekField<Options>(iconAndLabel),
          peekDelayField<Options>(iconAndLabel),
          field<&Options::tooltip>({
              .key = "tooltip",
          })
      ),
      .glyph = [](const Options& options) { return options.glyph; },
  };
  return definition;
}
