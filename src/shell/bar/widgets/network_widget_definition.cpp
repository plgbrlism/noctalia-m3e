#include "shell/bar/widgets/network_widget_definition.h"

#include "shell/bar/glyph_value_definition.h"

namespace {

  settings::WidgetSettingVisibility vpnLabelVisibility() {
    settings::WidgetSettingVisibility visibility;
    visibility.all = {
        {"show_label", {"true"}},
        {"vpn_status", {"replace", "both"}},
    };
    return visibility;
  }

} // namespace

const vespertine::bar::WidgetDefinition<NetworkWidget::Options>& networkWidgetDefinition() {
  using vespertine::bar::field;
  using Options = NetworkWidget::Options;

  static const vespertine::bar::WidgetDefinition<Options> definition{
      .type = "network",
      .fields = {
          field<&Options::vpnStatusMode>({
              .key = "vpn_status",
              .choices =
                  {
                      {
                          .value = VpnStatusMode::Replace,
                          .configValue = "replace",
                          .labelKey = "settings.widgets.options.replace",
                      },
                      {
                          .value = VpnStatusMode::Both,
                          .configValue = "both",
                          .labelKey = "settings.widgets.options.both",
                      },
                      {
                          .value = VpnStatusMode::Hidden,
                          .configValue = "hidden",
                          .labelKey = "settings.widgets.options.hidden",
                      },
                  },
          }),
          field<&Options::showLabel>({
              .key = "show_label",
          }),
          vespertine::bar::glyphPositionField<Options>(vespertine::bar::whenAllTrue({"show_label"})),
          vespertine::bar::peekField<Options>(vespertine::bar::whenAllTrue({"show_label"})),
          vespertine::bar::peekDelayField<Options>(vespertine::bar::whenAllTrue({"show_label"})),
          field<&Options::showVpnLabel>({
              .key = "show_vpn_label",
              .presentation = settings::WidgetSettingPresentation{
                  .visibleWhen = vpnLabelVisibility(),
              },
          }),
      },
  };
  return definition;
}
