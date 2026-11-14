#pragma once

#include "ui/controls/flex.h"
#include "ui/palette.h"

namespace panel_header_style {
  inline void applyHeaderSurface(Flex& header, float opacity) { header.setCardStyle(1.0F, opacity, false); }
} // namespace panel_header_style
