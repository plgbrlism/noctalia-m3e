#pragma once

#include "config/config_types.h"
#include "ui/controls/color_swatch_preview.h"

namespace vespertine::theme {
  struct AvailablePalette;
  struct BuiltinPalette;
} // namespace vespertine::theme

// Swatch strips shown next to palette names in pickers. Light mode previews the light variant; every other mode,
// including Auto, previews the dark one.
ColorSwatchPreview builtinPaletteSwatchPreview(const vespertine::theme::BuiltinPalette& entry, ThemeMode mode);
// Community and custom palettes carry preview metadata; a palette without a light preview falls back to its dark one.
ColorSwatchPreview availablePaletteSwatchPreview(const vespertine::theme::AvailablePalette& entry, ThemeMode mode);
