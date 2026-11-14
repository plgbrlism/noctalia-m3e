#pragma once

#include "ui/controls/button.h"
#include "ui/style.h"

namespace panel_button_style {

  inline constexpr float kIconButtonSize = 48.0F;
  inline constexpr float kIconButtonGlyphSize = 21.0F;

  inline void configureHeaderIconButton(Button& button, float scale, float surfaceOpacity) {
    const float size = kIconButtonSize * scale;
    button.setVariant(ButtonVariant::Ghost);
    button.setSurfaceOpacity(surfaceOpacity);
    button.setGlyphSize(kIconButtonGlyphSize * scale);
    button.setMinWidth(size);
    button.setMinHeight(size);
    button.setPadding(Style::spaceSm * scale, 0.0F);
    button.setRadius(Style::scaledRadiusLg(scale));
  }

} // namespace panel_button_style
