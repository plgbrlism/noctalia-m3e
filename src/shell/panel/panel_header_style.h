#pragma once

#include "shell/panel/panel_button_style.h"
#include "ui/controls/flex.h"
#include "ui/controls/label.h"
#include "ui/palette.h"

namespace panel_header_style {
  inline void applyHeaderSurface(Flex& header, float opacity, float scale) {
    header.setCardStyle(1.0F, opacity, false);
    for (auto& child : header.children()) {
      if (auto* label = dynamic_cast<Label*>(child.get())) {
        label->setFontSize(panel_button_style::kIconButtonGlyphSize * scale);
        label->setColor(colorSpecFromRole(ColorRole::Primary));
      }
    }
  }
} // namespace panel_header_style
