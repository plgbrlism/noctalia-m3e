#include "shell/bar/widgets/bluetooth_widget_definition.h"

#include "shell/bar/glyph_value_definition.h"

namespace {

  // A powered-off adapter has no connected device, so hiding on "no connected device"
  // already covers the adapter-off case and makes this toggle inert.
  settings::WidgetSettingVisibility connectedDeviceFilterOff() {
    settings::WidgetSettingVisibility visibility;
    visibility.all = {{"hide_when_no_connected_device", {"false"}}};
    return visibility;
  }

} // namespace

const vespertine::bar::WidgetDefinition<BluetoothWidget::Options>& bluetoothWidgetDefinition() {
  using vespertine::bar::field;
  using vespertine::bar::glyphPositionField;
  using vespertine::bar::peekDelayField;
  using vespertine::bar::peekField;
  using vespertine::bar::whenAllTrue;
  using Options = BluetoothWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "bluetooth",
      .fields = {
          field<&Options::showLabel>({
              .key = "show_label",
          }),
          glyphPositionField<Options>(whenAllTrue({"show_label"})),
          peekField<Options>(whenAllTrue({"show_label"})),
          peekDelayField<Options>(whenAllTrue({"show_label"})),
          field<&Options::hideWhenAdapterOff>({
              .key = "hide_when_adapter_off",
              .presentation =
                  settings::WidgetSettingPresentation{
                      .visibleWhen = connectedDeviceFilterOff(),
                  },
          }),
          field<&Options::hideWhenNoConnectedDevice>({
              .key = "hide_when_no_connected_device",
          }),
      },
  };
  return definition;
}
