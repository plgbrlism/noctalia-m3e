#pragma once

#include "config/config_service.h"
#include "shell/dialogs/dialog_popup_host.h"
#include "ui/style.h"

inline DialogPopupSettingsProvider dialogPopupSettings(const ConfigService& config) {
  return [&config] {
    const auto& cfg = config.config();
    return DialogPopupSettings{.shadow = cfg.shell.shadow, .uiScale = Style::densityScale()};
  };
}
