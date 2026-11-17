#include "ui/controls/slider.h"

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

Slider::Slider() {
  auto inactiveLeft = std::make_unique<Box>();
  m_inactiveTrackLeft = static_cast<Box*>(addChild(std::move(inactiveLeft)));
  m_inactiveTrackLeft->setParticipatesInLayout(false);

  auto active = std::make_unique<Box>();
  m_activeTrack = static_cast<Box*>(addChild(std::move(active)));
  m_activeTrack->setParticipatesInLayout(false);

  auto inactiveRight = std::make_unique<Box>();
  m_inactiveTrackRight = static_cast<Box*>(addChild(std::move(inactiveRight)));
  m_inactiveTrackRight->setParticipatesInLayout(false);

  auto handle = std::make_unique<Box>();
  m_handle = static_cast<Box*>(addChild(std::move(handle)));
  m_handle->setParticipatesInLayout(false);

  auto handle2 = std::make_unique<Box>();
  m_handle2 = static_cast<Box*>(addChild(std::move(handle2)));
  m_handle2->setParticipatesInLayout(false);

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
    if (!data.pressed) {
      applyVisualState();
      updateSquishState();
      markPaintDirty();
      if (m_onDragEnd) {
        m_onDragEnd();
      }
      return;
    }
    applyVisualState();
    updateSquishState();

    if (m_isRange) {
      const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth;
      const float trackX = Style::sliderHorizontalPadding;
      const float trackW = std::max(0.0F, widthPx - Style::sliderHorizontalPadding * 2.0F);
      const float t1 = static_cast<float>(std::clamp((m_value - m_min) / (m_max - m_min), 0.0, 1.0));
      const float t2 = static_cast<float>(std::clamp((m_value2 - m_min) / (m_max - m_min), 0.0, 1.0));
      const float center1 = trackX + (Style::rtl() ? 1.0F - t1 : t1) * trackW;
      const float center2 = trackX + (Style::rtl() ? 1.0F - t2 : t2) * trackW;
      const float dist1 = std::abs(data.localX - center1);
      const float dist2 = std::abs(data.localX - center2);
      m_activeHandle = (dist1 <= dist2) ? 1 : 2;
    } else {
      m_activeHandle = 1;
    }

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
    if (!m_enabled || !m_wheelAdjustEnabled || data.axis != WL_POINTER_AXIS_VERTICAL_SCROLL) {
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
    const double current = (m_isRange && m_activeHandle == 2) ? m_value2 : m_value;
    if ((increase && current >= m_max - kValueEpsilon) || (!increase && current <= m_min + kValueEpsilon)) {
      return false;
    }
    const float steps = data.scrollSteps();
    if (steps != 0.0F) {
      const double next = current - steps * step;
      if (m_isRange && m_activeHandle == 2) {
        setValue2(next);
      } else {
        setValue(next);
      }
      if (m_onDragEnd) {
        m_onDragEnd();
      }
    }
    return true;
  });
  area->setFocusable(true);
  area->setOnFocusGain([this] {
    applyVisualState();
    markPaintDirty();
  });
  area->setOnFocusLoss([this] {
    applyVisualState();
    markPaintDirty();
  });
  area->setOnKeyDown([this](const InputArea::KeyData& key) {
    if (!key.pressed || !m_enabled) {
      return;
    }
    const double step = m_step > 0.0 ? m_step : (m_max - m_min) * 0.05;
    if (step <= 0.0) {
      return;
    }
    const double current = (m_isRange && m_activeHandle == 2) ? m_value2 : m_value;
    double next = current;
    if (KeybindMatcher::matches(KeybindAction::Left, key.sym, key.modifiers)) {
      next = current + (Style::rtl() ? step : -step);
    } else if (KeybindMatcher::matches(KeybindAction::Right, key.sym, key.modifiers)) {
      next = current + (Style::rtl() ? -step : step);
    } else if (KeySymbol::isPageDown(key.sym)) {
      next = current - step * 10.0;
    } else if (KeySymbol::isPageUp(key.sym)) {
      next = current + step * 10.0;
    } else if (KeySymbol::isHome(key.sym)) {
      next = m_min;
    } else if (KeySymbol::isEnd(key.sym)) {
      next = m_max;
    } else {
      return;
    }
    if (m_isRange && m_activeHandle == 2) {
      setValue2(next);
    } else {
      setValue(next);
    }
    if (m_onDragEnd) {
      m_onDragEnd();
    }
  });
  m_inputArea = static_cast<InputArea*>(addChild(std::move(area)));
  m_inputArea->setParticipatesInLayout(false);
  m_inputArea->setCursorShape(WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_POINTER);

  applyVisualState();
}

Slider::~Slider() {
  if (m_squishAnimId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_squishAnimId);
    m_squishAnimId = 0;
  }
}

void Slider::setRange(double minValue, double maxValue) {
  if (maxValue < minValue) {
    std::swap(minValue, maxValue);
  }
  if (m_min == minValue && m_max == maxValue) {
    return;
  }
  m_min = minValue;
  m_max = maxValue;
  const double first = snapped(m_value);
  const double second = snapped(m_value2);
  const double low = std::min(first, second);
  const double high = std::max(first, second);
  const bool valueChanged = std::abs(low - m_value) >= kValueEpsilon || std::abs(high - m_value2) >= kValueEpsilon;
  m_value = low;
  m_value2 = high;
  updateGeometry();
  markPaintDirty();
  if (valueChanged) {
    if (m_onValueChanged) {
      m_onValueChanged(m_value);
    }
    if (m_isRange && m_onRangeChanged) {
      m_onRangeChanged(m_value, m_value2);
    }
  }
}

void Slider::setStep(double step) {
  m_step = std::max(step, 0.0);
  const double next = snapped(m_value);
  const bool valueChanged = std::abs(next - m_value) >= kValueEpsilon;
  m_value = next;
  updateGeometry();
  markPaintDirty();
  if (valueChanged && m_onValueChanged) {
    m_onValueChanged(m_value);
  }
}

void Slider::setValue(double value) {
  const double next = snapped(value);
  if (std::abs(next - m_value) < kValueEpsilon) {
    return;
  }
  m_value = next;
  if (m_isRange && m_value > m_value2) {
    m_value2 = m_value;
  }
  updateGeometry();
  markPaintDirty();
  if (m_onValueChanged) {
    m_onValueChanged(m_value);
  }
  if (m_isRange && m_onRangeChanged) {
    m_onRangeChanged(m_value, m_value2);
  }
}

void Slider::setRangeEnabled(bool enabled) {
  if (m_isRange == enabled) {
    return;
  }
  m_isRange = enabled;
  if (enabled && m_value > m_value2) {
    m_value2 = m_value;
  }
  applyVisualState();
  updateGeometry();
  markPaintDirty();
  if (enabled && m_onRangeChanged) {
    m_onRangeChanged(m_value, m_value2);
  }
}

void Slider::setValue2(double value) {
  const double next = snapped(value);
  if (std::abs(next - m_value2) < kValueEpsilon) {
    return;
  }
  m_value2 = next;
  if (m_isRange && m_value2 < m_value) {
    m_value = m_value2;
  }
  updateGeometry();
  markPaintDirty();
  if (m_onValueChanged) {
    m_onValueChanged(m_value2);
  }
  if (m_isRange && m_onRangeChanged) {
    m_onRangeChanged(m_value, m_value2);
  }
}

void Slider::setRangeValues(double low, double high) {
  double nextLow = snapped(low);
  double nextHigh = snapped(high);
  if (nextLow > nextHigh) {
    std::swap(nextLow, nextHigh);
  }
  if (std::abs(nextLow - m_value) < kValueEpsilon && std::abs(nextHigh - m_value2) < kValueEpsilon) {
    return;
  }
  m_value = nextLow;
  m_value2 = nextHigh;
  updateGeometry();
  markPaintDirty();
  if (m_onValueChanged) {
    m_onValueChanged(m_value);
  }
  if (m_onRangeChanged) {
    m_onRangeChanged(m_value, m_value2);
  }
}

void Slider::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  applyVisualState();
  markPaintDirty();
}

void Slider::setTrackHeight(float height) {
  m_trackHeight = std::max(1.0F, height);
  updateGeometry();
  markLayoutDirty();
}

void Slider::setThumbSize(float size) {
  m_thumbSizePx = std::max(1.0F, size);
  updateGeometry();
  markLayoutDirty();
}

void Slider::setControlHeight(float height) {
  m_controlHeightPx = std::max(1.0F, height);
  updateGeometry();
  markLayoutDirty();
}

void Slider::setScale(float scale) {
  m_scale = std::max(0.01F, scale);
  m_trackHeight = Style::sliderTrackHeight * m_scale;
  m_controlHeightPx = Style::sliderHandleHeight * m_scale;
  updateGeometry();
  markLayoutDirty();
}

void Slider::setWheelAdjustEnabled(bool enabled) { m_wheelAdjustEnabled = enabled; }

void Slider::setOnValueChanged(std::function<void(double)> callback) { m_onValueChanged = std::move(callback); }

void Slider::setOnRangeChanged(std::function<void(double, double)> callback) {
  m_onRangeChanged = std::move(callback);
}

void Slider::setOnDragEnd(std::function<void()> callback) { m_onDragEnd = std::move(callback); }

bool Slider::dragging() const noexcept { return m_inputArea != nullptr && m_inputArea->pressed(); }

void Slider::doLayout(Renderer& /*renderer*/) {
  updateGeometry();
  applyVisualState();
}

LayoutSize Slider::doMeasure(Renderer& renderer, const LayoutConstraints& constraints) {
  return measureByLayout(renderer, constraints);
}

void Slider::doArrange(Renderer& renderer, const LayoutRect& rect) { arrangeByLayout(renderer, rect); }

void Slider::updateSquishState() {
  if (animationManager() == nullptr || !m_enabled) {
    m_handleSquish = 0.0F;
    applyVisualState();
    return;
  }
  if (m_squishAnimId != 0) {
    animationManager()->cancel(m_squishAnimId);
    m_squishAnimId = 0;
  }
  const float target = pressed() ? 1.0F : 0.0F;
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
        markPaintDirty();
      },
      [this] {
        m_squishAnimId = 0;
      },
      this
  );
  markPaintDirty();
}

void Slider::updateGeometry() {
  const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth * m_scale;
  const float heightPx = std::max({Style::sliderHandleHeight * m_scale, m_trackHeight, m_controlHeightPx});
  setSize(widthPx, heightPx);

  const float trackY = (heightPx - m_trackHeight) * 0.5F;
  const float trackX = Style::sliderHorizontalPadding * m_scale;
  const float trackW = std::max(0.0F, widthPx - trackX * 2.0F);

  const float outerR = Style::sliderTrackRadiusOuter * m_scale;
  const float innerR = Style::sliderTrackRadiusInner * m_scale;
  const auto clampR = [](float radius, float segW) { return std::min(radius, segW * 0.5F); };

  const float thumbWidth =
      std::lerp(Style::sliderHandleWidth, Style::sliderHandlePressedWidth, m_handleSquish) * m_scale;
  const float thumbHeight = Style::sliderHandleHeight * m_scale;
  const float thumbY = (heightPx - thumbHeight) * 0.5F;
  const float gap = Style::sliderGap * m_scale;
  const float minCenter = trackX + thumbWidth * 0.5F;
  const float maxCenter = trackX + trackW - thumbWidth * 0.5F;

  const double norm1 = std::clamp((m_value - m_min) / (m_max - m_min), 0.0, 1.0);
  const float t1 = static_cast<float>(norm1);
  const float t1Vis = Style::rtl() ? 1.0F - t1 : t1;
  const float center1 = std::clamp(trackX + t1Vis * trackW, minCenter, maxCenter);
  const float thumbX1 = center1 - thumbWidth * 0.5F;

  const float dotSize = Style::sliderDotSize * m_scale;
  const float dotInset = Style::sliderDotInset * m_scale;

  if (!m_isRange) {
    m_inactiveTrackLeft->setVisible(false);
    m_handle2->setVisible(false);

    const float activeWidth = std::max(0.0F, center1 - thumbWidth * 0.5F - gap - trackX);
    const float inactiveStart = center1 + thumbWidth * 0.5F + gap;
    const float inactiveWidth = std::max(0.0F, (trackX + trackW) - inactiveStart);

    m_activeTrack->setVisible(activeWidth > 0.0F && m_enabled);
    m_activeTrack->setPosition(trackX, trackY);
    m_activeTrack->setFrameSize(activeWidth, m_trackHeight);
    m_activeTrack->setRadii(Radii{
        clampR(outerR, activeWidth), clampR(innerR, activeWidth), clampR(innerR, activeWidth),
        clampR(outerR, activeWidth)
    });

    m_inactiveTrackRight->setVisible(inactiveWidth > 0.0F && m_enabled);
    m_inactiveTrackRight->setPosition(inactiveStart, trackY);
    m_inactiveTrackRight->setFrameSize(inactiveWidth, m_trackHeight);
    m_inactiveTrackRight->setRadii(Radii{
        clampR(innerR, inactiveWidth), clampR(outerR, inactiveWidth), clampR(outerR, inactiveWidth),
        clampR(innerR, inactiveWidth)
    });

    m_handle->setVisible(m_enabled);
    m_handle->setPosition(thumbX1, thumbY);
    m_handle->setFrameSize(thumbWidth, thumbHeight);
    m_handle->setRadius(thumbWidth * 0.5F);

    const bool showDotRight = inactiveWidth >= (dotInset + dotSize);
    m_dotRight->setVisible(showDotRight && m_enabled);
    if (showDotRight) {
      m_dotRight->setPosition(trackX + trackW - dotInset - dotSize, trackY + (m_trackHeight - dotSize) * 0.5F);
      m_dotRight->setFrameSize(dotSize, dotSize);
      m_dotRight->setRadius(dotSize * 0.5F);
    }
    m_dotLeft->setVisible(false);
  } else {
    const double norm2 = std::clamp((m_value2 - m_min) / (m_max - m_min), 0.0, 1.0);
    const float t2 = static_cast<float>(norm2);
    const float t2Vis = Style::rtl() ? 1.0F - t2 : t2;
    const float center2Raw = std::clamp(trackX + t2Vis * trackW, minCenter, maxCenter);

    const bool firstIsLow = center1 <= center2Raw;
    float lowCenter = std::min(center1, center2Raw);
    float highCenter = std::max(center1, center2Raw);
    const float minSep = thumbWidth + gap;
    if (highCenter - lowCenter < minSep) {
      const float mid = (lowCenter + highCenter) * 0.5F;
      lowCenter = std::max(minCenter, mid - minSep * 0.5F);
      highCenter = std::min(maxCenter, lowCenter + minSep);
      if (highCenter - lowCenter < minSep) {
        lowCenter = std::max(minCenter, maxCenter - minSep);
        highCenter = std::min(maxCenter, lowCenter + minSep);
      }
    }
    const float adjCenter1 = firstIsLow ? lowCenter : highCenter;
    const float adjCenter2 = firstIsLow ? highCenter : lowCenter;
    const float thumbX2 = adjCenter2 - thumbWidth * 0.5F;

    const float leftW = std::max(0.0F, lowCenter - thumbWidth * 0.5F - gap - trackX);
    const float activeStart = lowCenter + thumbWidth * 0.5F + gap;
    const float activeW = std::max(0.0F, highCenter - thumbWidth * 0.5F - gap - activeStart);
    const float rightStart = highCenter + thumbWidth * 0.5F + gap;
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

    m_handle->setVisible(m_enabled);
    m_handle->setPosition(adjCenter1 - thumbWidth * 0.5F, thumbY);
    m_handle->setFrameSize(thumbWidth, thumbHeight);
    m_handle->setRadius(thumbWidth * 0.5F);

    m_handle2->setVisible(m_enabled);
    m_handle2->setPosition(thumbX2, thumbY);
    m_handle2->setFrameSize(thumbWidth, thumbHeight);
    m_handle2->setRadius(thumbWidth * 0.5F);

    const bool showDotLeft = leftW >= (dotInset + dotSize);
    const bool showDotRight = rightW >= (dotInset + dotSize);
    m_dotLeft->setVisible(showDotLeft && m_enabled);
    if (showDotLeft) {
      m_dotLeft->setPosition(trackX + dotInset, trackY + (m_trackHeight - dotSize) * 0.5F);
      m_dotLeft->setFrameSize(dotSize, dotSize);
      m_dotLeft->setRadius(dotSize * 0.5F);
    }
    m_dotRight->setVisible(showDotRight && m_enabled);
    if (showDotRight) {
      m_dotRight->setPosition(trackX + trackW - dotInset - dotSize, trackY + (m_trackHeight - dotSize) * 0.5F);
      m_dotRight->setFrameSize(dotSize, dotSize);
      m_dotRight->setRadius(dotSize * 0.5F);
    }
  }

  m_inputArea->setPosition(0.0F, 0.0F);
  m_inputArea->setFrameSize(widthPx, heightPx);
}

void Slider::updateFromLocalX(float x) {
  const float widthPx = width() > 0.0F ? width() : Style::sliderDefaultWidth * m_scale;
  const float trackX = Style::sliderHorizontalPadding * m_scale;
  const float trackW = std::max(0.0F, widthPx - trackX * 2.0F);
  if (trackW <= 0.0F) {
    return;
  }
  const float raw = std::clamp((x - trackX) / trackW, 0.0F, 1.0F);
  const double t = Style::rtl() ? 1.0 - static_cast<double>(raw) : static_cast<double>(raw);
  const double targetVal = m_min + t * (m_max - m_min);

  if (m_isRange) {
    if (m_activeHandle == 2) {
      setValue2(targetVal);
    } else {
      setValue(targetVal);
    }
  } else {
    setValue(targetVal);
  }
}

void Slider::applyVisualState() {
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
  m_handle->setFill(handleColor);
  m_handle2->setFill(handleColor);
  m_dotLeft->setFill(dotColor);
  m_dotRight->setFill(dotColor);

  markPaintDirty();
}

float Slider::normalizedValue() const noexcept {
  if (m_max <= m_min) {
    return 0.0F;
  }
  return static_cast<float>(std::clamp((m_value - m_min) / (m_max - m_min), 0.0, 1.0));
}

double Slider::snapped(double value) const noexcept {
  const double clamped = std::clamp(value, m_min, m_max);
  if (m_step <= 0.0 || m_max <= m_min) {
    return clamped;
  }

  const double steps = std::round((clamped - m_min) / m_step);
  return std::clamp(m_min + steps * m_step, m_min, m_max);
}
