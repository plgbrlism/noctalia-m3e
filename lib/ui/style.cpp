#include "ui/style.h"

#include <algorithm>

namespace {

  Style::CornerShapePreset g_cornerShape = Style::CornerShapePreset::Standard;
  bool g_buttonBordersEnabled = true;
  bool g_inputBordersEnabled = true;
  bool g_popupBordersEnabled = true;
  bool g_rtl = false;
  bool g_popupShadowsEnabled = true;
  bool g_cardBordersEnabled = true;
  bool g_closeButtonsEnabled = true;
  Style::DesignSystem g_designSystem = Style::DesignSystem::Expressive;
  Style::CornerStyle g_cornerStyle = Style::CornerStyle::Material;
  Style::MotionPreset g_motionPreset = Style::MotionPreset::Balanced;

} // namespace

namespace Style {

  CornerShapePreset cornerShape() noexcept { return g_cornerShape; }

  void setCornerShape(CornerShapePreset shape) noexcept {
    if (g_cornerShape != shape) {
      g_cornerShape = shape;
      cornerShapeChanged().emit();
    }
  }

  Signal<>& cornerShapeChanged() {
    static Signal<> s;
    return s;
  }

  bool buttonBordersEnabled() noexcept { return g_buttonBordersEnabled; }

  void setButtonBordersEnabled(bool enabled) {
    if (g_buttonBordersEnabled == enabled) {
      return;
    }
    g_buttonBordersEnabled = enabled;
    buttonBordersChanged().emit();
  }

  Signal<>& buttonBordersChanged() {
    static Signal<> signal;
    return signal;
  }

  bool inputBordersEnabled() noexcept { return g_inputBordersEnabled; }

  void setInputBordersEnabled(bool enabled) {
    if (g_inputBordersEnabled == enabled) {
      return;
    }
    g_inputBordersEnabled = enabled;
    inputBordersChanged().emit();
  }

  Signal<>& inputBordersChanged() {
    static Signal<> signal;
    return signal;
  }

  bool popupBordersEnabled() noexcept { return g_popupBordersEnabled; }
  void setPopupBordersEnabled(bool enabled) { g_popupBordersEnabled = enabled; }

  bool rtl() noexcept { return g_rtl; }
  void setRtl(bool rtl) noexcept { g_rtl = rtl; }
  bool cardBordersEnabled() noexcept { return g_cardBordersEnabled; }
  void setCardBordersEnabled(bool enabled) { g_cardBordersEnabled = enabled; }

  bool popupShadowsEnabled() noexcept { return g_popupShadowsEnabled; }
  void setPopupShadowsEnabled(bool enabled) { g_popupShadowsEnabled = enabled; }

  bool closeButtonsEnabled() noexcept { return g_closeButtonsEnabled; }
  void setCloseButtonsEnabled(bool enabled) { g_closeButtonsEnabled = enabled; }

  float scaledRadius(float radius, float localScale) noexcept {
    const float s = (g_cornerShape == CornerShapePreset::Sharp) ? 0.0F
                  : (g_cornerShape == CornerShapePreset::Round) ? 2.0F
                  : 1.0F;
    return radius * localScale * s;
  }

  float scaledRadiusSm(float localScale) noexcept { return scaledRadius(radiusSm, localScale); }

  float scaledRadiusMd(float localScale) noexcept { return scaledRadius(radiusMd, localScale); }

  float scaledRadiusLg(float localScale) noexcept { return scaledRadius(radiusLg, localScale); }

  float scaledRadiusXl(float localScale) noexcept { return scaledRadius(radiusXl, localScale); }

  DesignSystem designSystem() noexcept { return g_designSystem; }

  void setDesignSystem(DesignSystem ds) noexcept {
    if (g_designSystem != ds) {
      g_designSystem = ds;
      designSystemChanged().emit();
    }
  }

  Signal<>& designSystemChanged() {
    static Signal<> s;
    return s;
  }

  CornerStyle cornerStyle() noexcept { return g_cornerStyle; }

  void setCornerStyle(CornerStyle cs) noexcept {
    if (g_cornerStyle != cs) {
      g_cornerStyle = cs;
      cornerStyleChanged().emit();
    }
  }

  Signal<>& cornerStyleChanged() {
    static Signal<> s;
    return s;
  }

  MotionPreset motionPreset() noexcept { return g_motionPreset; }

  void setMotionPreset(MotionPreset p) noexcept { g_motionPreset = p; }

} // namespace Style
