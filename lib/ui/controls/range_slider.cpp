#include "ui/controls/range_slider.h"

#include "core/input/key_modifiers.h"
#include "core/input/key_symbols.h"
#include "core/input/keybind_matcher.h"
#include "cursor-shape-v1-client-protocol.h"
#include "render/animation/animation_manager.h"
#include "render/core/render_styles.h"
#include "render/scene/input_area.h"
#include "ui/controls/box.h"
#include "ui/palette.h"
#include "ui/style.h"
#include "util/clamp.h"

#include <algorithm>
#include <cmath>
#include <linux/input-event-codes.h>
#include <memory>

namespace {

  constexpr double kValueEpsilon = 0.0001;

  Color resolved(ColorRole role, float alpha = 1.0F) { return colorForRole(role, alpha); }

} // namespace

RangeSlider::RangeSlider() {
  auto inactiveLeft = std::make_unique<Box>();
  m_inactiveTrackLeft = static_cast<Box*>(addChild(std::move(inactiveLeft)));
  m_inactiveTrackLeft->setParticipatesInLayout(false);

  auto active = std::make_unique<Box>();
  m_activeTrack = static_cast<Box*>(addChild(std::move(active)));
  m_activeTrack->setParticipatesInLayout(false);

  auto inactiveRight = std::make_unique<Box>();
  m_inactiveTrackRight = static_cast<Box*>(addChild(std::move(inactiveRight)));
  m_inactiveTrackRight->setParticipatesInLayout(false);

  auto lowHandle = std::make_unique<Box>();
  m_lowHandle = static_cast<Box*>(addChild(std::move(lowHandle)));
  m_lowHandle->setParticipatesInLayout(false);

  auto highHandle = std::make_unique<Box>();
  m_highHandle = static_cast<Box*>(addChild(std::move(highHandle)));
  m_highHandle->setParticipatesInLayout(false);

  auto dotLeft = std::make_unique<Box>();
  m_dotLeft = static_cast<Box*>(addChild(std::move(dotLeft)));
  m_dotLeft->setParticipatesInLayout(false);

  auto dotRight = std::make_unique<Box>();
  m_dotRight = static_cast<Box*>(addChild(std::move(dotRight)));
  m_dotRight->setParticipatesInLayout(false);

  auto area = std::make_unique<InputArea>();
  area->setOnPress([this](const InputArea::PointerData& data) {
    if (!m_enabled || data.button != BTN_LEFT) {
      return;
    }
    m_pressed = data.pressed;
    updateSquishState();
    if (!data.pressed) {
      m_activeThumb = ActiveThumb::None;
      applyVisualState();
      markPaintDirty();
      if (m_onDragEnd) {
        m_onDragEnd();
      }
      return;
    }
    // Pick the thumb nearest the press; ties (equal values) grab whichever side the press lands on.
    const float lowX = thumbCenterX(m_low);
    const float highX = thumbCenterX(m_high);
    if (std::abs(data.localX - lowX) < std::abs(data.localX - highX)) {
      m_activeThumb = ActiveThumb::Low;
    } else if (std::abs(data.localX - lowX) > std::abs(data.localX - highX)) {
      m_activeThumb = ActiveThumb::High;
    } else {
      m_activeThumb = data.localX < lowX ? ActiveThumb::Low : ActiveThumb::High;
    }
    applyVisualState();
    updateFromLocalX(data.localX);
    markPaintDirty();
  });
  area->setOnMotion([this](const InputArea::PointerData& data) {
    if (!m_enabled || m_inputArea == nullptr || !m_inputArea->pressed()) {
      return;
    }
    updateFromLocalX(data.localX);
  });
  area->setOnAxisHandler([this](const InputArea::PointerData& data) {
    if (!m_enabled || data.axis != WL_POINTER_AXIS_VERTICAL_SCROLL) {
      return false;
    }
    const float frameDelta = data.axisLines != 0.0F ? data.axisLines : static_cast<float>(data.axisValue);
    if (frameDelta == 0.0F) {
      return false;
    }
    const double step = m_step > 0.0 ? m_step : (m_max - m_min) * 0.05;
    if (step <= 0.0) {
      return false;
    }
    const bool increase = frameDelta < 0.0F;
    // Wheel drives the thumb nearest the pointer; a pinned thumb declines so ancestor views scroll.
    const float lowX = thumbCenterX(m_low);
    const float highX = thumbCenterX(m_high);
    // On a tie (thumbs coincide) the pointer side picks, mirroring the press handler.
    const bool adjustLow = std::abs(data.localX - lowX) < std::abs(data.localX - highX)
        || (std::abs(data.localX - lowX) == std::abs(data.localX - highX) && data.localX < lowX);
    if (adjustLow) {
      if ((increase && m_low >= m_high - kValueEpsilon) || (!increase && m_low <= m_min + kValueEpsilon)) {
        return false;
      }
    } else {
      if ((increase && m_high >= m_max - kValueEpsilon) || (!increase && m_high <= m_low + kValueEpsilon)) {
        return false;
      }
    }
    const float steps = data.scrollSteps();
    if (steps != 0.0F) {
      if (adjustLow) {
        setLowValue(m_low - steps * step);
      } else {
        setHighValue(m_high - steps * step);
      }
      if (m_onDragEnd) {
        m_onDragEnd();
      }
    }
    return true;
  });
  area->setFocusable(true);
  area->setOnFocusGain([this] {
    if (m_activeThumb == ActiveThumb::None) {
      m_activeThumb = ActiveThumb::Low;
    }
    applyVisualState();
    markPaintDirty();
  });
  area->setOnFocusLoss([this] {
    m_activeThumb = ActiveThumb::None;
    applyVisualState();
    markPaintDirty();
  });
  area->setOnKeyDown([this](const InputArea::KeyData& key) {
    if (!key.pressed || !m_enabled) {
      return;
    }
    const bool adjustHigh = (key.modifiers & KeyMod::Shift) != 0;
    m_activeThumb = adjustHigh ? ActiveThumb::High : ActiveThumb::Low;
    const double step = m_step > 0.0 ? m_step : (m_max - m_min) * 0.05;
    if (step <= 0.0) {
      return;
    }
    auto nudge = [&](double delta) {
      if (adjustHigh) {
        setHighValue(m_high + delta);
      } else {
        setLowValue(m_low + delta);
      }
      if (m_onDragEnd) {
        m_onDragEnd();
      }
    };
    if (KeybindMatcher::matches(KeybindAction::Left, key.sym, key.modifiers)) {
      nudge(Style::rtl() ? step : -step);
    } else if (KeybindMatcher::matches(KeybindAction::Right, key.sym, key.modifiers)) {
      nudge(Style::rtl() ? -step : step);
    } else if (KeySymbol::isPageDown(key.sym)) {
      nudge(-step * 10.0);
    } else if (KeySymbol::isPageUp(key.sym)) {
      nudge(step * 10.0);
    } else if (KeySymbol::isHome(key.sym)) {
      if (adjustHigh) {
        setHighValue(m_low);
      } else {
        setLowValue(m_min);
      }
      if (m_onDragEnd) {
        m_onDragEnd();
      }
    } else if (KeySymbol::isEnd(key.sym)) {
      if (adjustHigh) {
        setHighValue(m_max);
      } else {
        setLowValue(m_high);
      }
      if (m_onDragEnd) {
        m_onDragEnd();
      }
    }
    applyVisualState();
    markPaintDirty();
  });
  m_inputArea = static_cast<InputArea*>(addChild(std::move(area)));
  m_inputArea->setParticipatesInLayout(false);
  m_inputArea->setCursorShape(WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER);

  applyVisualState();
}

RangeSlider::~RangeSlider() {
  if (m_squishAnimId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_squishAnimId);
    m_squishAnimId = 0;
  }
}

void RangeSlider::updateSquishState() {
  if (animationManager() == nullptr || !m_enabled) {
    m_handleSquish = 0.0F;
    applyVisualState();
    return;
  }
  if (m_squishAnimId != 0) {
    animationManager()->cancel(m_squishAnimId);
    m_squishAnimId = 0;
  }
  const float target = m_pressed ? 1.0F : 0.0F;
  if (std::abs(m_handleSquish - target) < 0.001F) {
    m_handleSquish = target;
    applyVisualState();
    return;
  }
  const float from = m_handleSquish;
  m_squishAnimId = animationManager()->animate(
      from, target, Style::animFast, Easing::EaseOutCubic,
      [this](float s) {
        m_handleSquish = s;
        updateGeometry();
        markLayoutDirty();
      },
      [this] {
        m_squishAnimId = 0;
      },
      this
  );
  markPaintDirty();
}

void RangeSlider::setScale(float scale) {
  m_scale = std::max(0.01F, scale);
  m_trackHeight = Style::sliderTrackHeight * m_scale;
  m_controlHeightPx = Style::sliderHandleHeight * m_scale;
  updateGeometry();
  markLayoutDirty();
}

void RangeSlider::setRange(double minValue, double maxValue) {
  if (maxValue < minValue) {
    std::swap(minValue, maxValue);
  }
  if (m_min == minValue && m_max == maxValue) {
    return;
  }
  m_min = minValue;
  m_max = maxValue;
  m_low = snapped(m_low);
  m_high = std::max(m_low, snapped(m_high));
  updateGeometry();
  markPaintDirty();
}

void RangeSlider::setStep(double step) {
  m_step = std::max(step, 0.0);
  m_low = snapped(m_low);
  m_high = std::max(m_low, snapped(m_high));
  updateGeometry();
  markPaintDirty();
}

void RangeSlider::setLowValue(double value) {
  const double next = std::min(snapped(value), m_high);
  if (std::abs(next - m_low) < kValueEpsilon) {
    return;
  }
  m_low = next;
  updateGeometry();
  markPaintDirty();
  if (m_onLowChanged) {
    m_onLowChanged(m_low);
  }
}

void RangeSlider::setHighValue(double value) {
  const double next = std::max(snapped(value), m_low);
  if (std::abs(next - m_high) < kValueEpsilon) {
    return;
  }
  m_high = next;
  updateGeometry();
  markPaintDirty();
  if (m_onHighChanged) {
    m_onHighChanged(m_high);
  }
}

void RangeSlider::setValues(double low, double high) {
  double lo = snapped(low);
  double hi = snapped(high);
  if (hi < lo) {
    std::swap(lo, hi);
  }
  if (std::abs(lo - m_low) < kValueEpsilon && std::abs(hi - m_high) < kValueEpsilon) {
    return;
  }
  m_low = lo;
  m_high = hi;
  updateGeometry();
  markPaintDirty();
}

void RangeSlider::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  applyVisualState();
  markPaintDirty();
}

void RangeSlider::setTrackHeight(float height) {
  m_trackHeight = std::max(1.0F, height);
  updateGeometry();
  markLayoutDirty();
}

void RangeSlider::setThumbSize(float size) {
  m_thumbSizePx = std::max(1.0F, size);
  updateGeometry();
  markLayoutDirty();
}

void RangeSlider::setControlHeight(float height) {
  m_controlHeightPx = std::max(1.0F, height);
  updateGeometry();
  markLayoutDirty();
}

void RangeSlider::setOnLowChanged(std::function<void(double)> callback) { m_onLowChanged = std::move(callback); }

void RangeSlider::setOnHighChanged(std::function<void(double)> callback) { m_onHighChanged = std::move(callback); }

void RangeSlider::setOnDragEnd(std::function<void()> callback) { m_onDragEnd = std::move(callback); }

bool RangeSlider::dragging() const noexcept { return m_inputArea != nullptr && m_inputArea->pressed(); }

void RangeSlider::doLayout(Renderer& /*renderer*/) {
  updateGeometry();
  applyVisualState();
}

LayoutSize RangeSlider::doMeasure(Renderer& renderer, const LayoutConstraints& constraints) {
  return measureByLayout(renderer, constraints);
}

void RangeSlider::doArrange(Renderer& renderer, const LayoutRect& rect) { arrangeByLayout(renderer, rect); }

void RangeSlider::updateGeometry() {
  const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth * m_scale;
  const float heightPx = std::max({Style::sliderHandleHeight * m_scale, m_trackHeight, m_controlHeightPx});
  setSize(widthPx, heightPx);

  const float trackY = (heightPx - m_trackHeight) * 0.5F;
  const float trackX = Style::sliderHorizontalPadding * m_scale;
  const float trackW = std::max(0.0F, widthPx - trackX * 2.0F);

  const float outerR = Style::sliderTrackRadiusOuter * m_scale;
  const float innerR = Style::sliderTrackRadiusInner * m_scale;
  const auto clampR = [](float radius, float segW) { return std::min(radius, segW * 0.5F); };

  const float hw =
      std::lerp(Style::sliderHandleWidth, Style::sliderHandlePressedWidth, m_handleSquish) * m_scale;
  const float hh = Style::sliderHandleHeight * m_scale;
  const float handleY = (heightPx - hh) * 0.5F;
  const float gap = Style::sliderGap * m_scale;
  const float minCenter = trackX + hw * 0.5F;
  const float maxCenter = trackX + trackW - hw * 0.5F;

  const float lowT = normalized(m_low);
  const float highT = normalized(m_high);
  const float lowRaw = trackX + (Style::rtl() ? 1.0F - lowT : lowT) * trackW;
  const float highRaw = trackX + (Style::rtl() ? 1.0F - highT : highT) * trackW;
  float lowCenter = std::clamp(lowRaw, minCenter, maxCenter);
  float highCenter = std::clamp(highRaw, minCenter, maxCenter);

  const float minSep = hw + gap;
  if (highCenter - lowCenter < minSep) {
    const float mid = (lowCenter + highCenter) * 0.5F;
    lowCenter = std::max(minCenter, mid - minSep * 0.5F);
    highCenter = std::min(maxCenter, lowCenter + minSep);
    if (highCenter - lowCenter < minSep) {
      lowCenter = std::max(minCenter, maxCenter - minSep);
      highCenter = std::min(maxCenter, lowCenter + minSep);
    }
  }

  const float leftW = std::max(0.0F, lowCenter - hw * 0.5F - gap - trackX);
  const float activeStart = lowCenter + hw * 0.5F + gap;
  const float activeW = std::max(0.0F, highCenter - hw * 0.5F - gap - activeStart);
  const float rightStart = highCenter + hw * 0.5F + gap;
  const float rightW = std::max(0.0F, (trackX + trackW) - rightStart);

  m_inactiveTrackLeft->setVisible(leftW > 0.0F && m_enabled);
  m_inactiveTrackLeft->setPosition(trackX, trackY);
  m_inactiveTrackLeft->setFrameSize(leftW, m_trackHeight);
  m_inactiveTrackLeft->setRadii(Radii{
      clampR(outerR, leftW), clampR(innerR, leftW), clampR(innerR, leftW), clampR(outerR, leftW)
  });

  m_activeTrack->setVisible(activeW > 0.0F && m_enabled);
  m_activeTrack->setPosition(activeStart, trackY);
  m_activeTrack->setFrameSize(activeW, m_trackHeight);
  m_activeTrack->setRadii(Radii{
      clampR(innerR, activeW), clampR(innerR, activeW), clampR(innerR, activeW), clampR(innerR, activeW)
  });

  m_inactiveTrackRight->setVisible(rightW > 0.0F && m_enabled);
  m_inactiveTrackRight->setPosition(rightStart, trackY);
  m_inactiveTrackRight->setFrameSize(rightW, m_trackHeight);
  m_inactiveTrackRight->setRadii(Radii{
      clampR(innerR, rightW), clampR(outerR, rightW), clampR(outerR, rightW), clampR(innerR, rightW)
  });

  m_lowHandle->setVisible(m_enabled);
  m_lowHandle->setPosition(lowCenter - hw * 0.5F, handleY);
  m_lowHandle->setFrameSize(hw, hh);
  m_lowHandle->setRadius(hw * 0.5F);

  m_highHandle->setVisible(m_enabled);
  m_highHandle->setPosition(highCenter - hw * 0.5F, handleY);
  m_highHandle->setFrameSize(hw, hh);
  m_highHandle->setRadius(hw * 0.5F);

  const float dotSize = Style::sliderDotSize * m_scale;
  const float dotInset = Style::sliderDotInset * m_scale;

  const bool showDotLeft = leftW >= (dotInset + dotSize);
  m_dotLeft->setVisible(showDotLeft && m_enabled);
  if (showDotLeft) {
    m_dotLeft->setPosition(trackX + dotInset, trackY + (m_trackHeight - dotSize) * 0.5F);
    m_dotLeft->setFrameSize(dotSize, dotSize);
    m_dotLeft->setRadius(dotSize * 0.5F);
  }

  const bool showDotRight = rightW >= (dotInset + dotSize);
  m_dotRight->setVisible(showDotRight && m_enabled);
  if (showDotRight) {
    m_dotRight->setPosition(trackX + trackW - dotInset - dotSize, trackY + (m_trackHeight - dotSize) * 0.5F);
    m_dotRight->setFrameSize(dotSize, dotSize);
    m_dotRight->setRadius(dotSize * 0.5F);
  }

  m_inputArea->setPosition(0.0F, 0.0F);
  m_inputArea->setFrameSize(widthPx, heightPx);
}

void RangeSlider::updateFromLocalX(float x) {
  const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth * m_scale;
  const float trackX = Style::sliderHorizontalPadding * m_scale;
  const float trackW = std::max(0.0F, widthPx - trackX * 2.0F);
  if (trackW <= 0.0F || m_activeThumb == ActiveThumb::None) {
    return;
  }
  const double raw = static_cast<double>(std::clamp((x - trackX) / trackW, 0.0F, 1.0F));
  const double t = Style::rtl() ? 1.0 - raw : raw;
  const double value = m_min + t * (m_max - m_min);
  if (m_activeThumb == ActiveThumb::Low) {
    setLowValue(value);
  } else {
    setHighValue(value);
  }
}

void RangeSlider::applyVisualState() {
  Color inactiveTrackColor = resolved(ColorRole::SecondaryContainer);
  Color activeTrackColor = resolved(ColorRole::Primary);
  Color handleColor = resolved(ColorRole::Primary);
  Color dotColor = resolved(ColorRole::Primary);

  if (!m_enabled) {
    inactiveTrackColor = resolved(ColorRole::SecondaryContainer, 0.12F);
    activeTrackColor = resolved(ColorRole::Primary, 0.38F);
    handleColor = resolved(ColorRole::Primary, 0.38F);
    dotColor = resolved(ColorRole::Primary, 0.38F);
  }

  m_inactiveTrackLeft->setFill(inactiveTrackColor);
  m_inactiveTrackRight->setFill(inactiveTrackColor);
  m_activeTrack->setFill(activeTrackColor);
  m_lowHandle->setFill(handleColor);
  m_highHandle->setFill(handleColor);
  m_dotLeft->setFill(dotColor);
  m_dotRight->setFill(dotColor);

  markPaintDirty();
}

float RangeSlider::normalized(double value) const noexcept {
  if (m_max <= m_min) {
    return 0.0F;
  }
  return static_cast<float>(std::clamp((value - m_min) / (m_max - m_min), 0.0, 1.0));
}

float RangeSlider::thumbCenterX(double value) const noexcept {
  const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth * m_scale;
  const float trackX = Style::sliderHorizontalPadding * m_scale;
  const float trackW = std::max(0.0F, widthPx - trackX * 2.0F);
  const float hw =
      std::lerp(Style::sliderHandleWidth, Style::sliderHandlePressedWidth, m_handleSquish) * m_scale;
  const float t = normalized(value);
  const float raw = trackX + (Style::rtl() ? 1.0F - t : t) * trackW;
  return std::clamp(raw, trackX + hw * 0.5F, trackX + trackW - hw * 0.5F);
}

double RangeSlider::snapped(double value) const noexcept {
  const double clamped = std::clamp(value, m_min, m_max);
  if (m_step <= 0.0 || m_max <= m_min) {
    return clamped;
  }
  const double steps = std::round((clamped - m_min) / m_step);
  return std::clamp(m_min + steps * m_step, m_min, m_max);
}
