#include "ui/controls/segmented.h"

#include "render/core/render_styles.h"
#include "render/scene/input_area.h"
#include "ui/controls/button.h"
#include "ui/controls/flex.h"
#include "ui/controls/roving_list_nav.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <algorithm>
#include <memory>
#include <utility>

Segmented::Segmented() {
  setDirection(FlexDirection::Horizontal);
  setAlign(FlexAlign::Stretch);
  setGap(6.0F * m_scale);
  clearFill();
  clearBorder();

  auto area = std::make_unique<InputArea>();
  area->setFocusable(true);
  area->setHitTestVisible(false);
  m_focusArea = static_cast<InputArea*>(addChild(std::move(area)));
  m_focusArea->setParticipatesInLayout(false);
  m_focusArea->setZIndex(2);

  m_rovingNav.setOptions(
      RovingListNavController::Options{
          .axis = RovingListNavAxis::Horizontal,
          .mode = RovingListNavMode::FollowFocus,
          .scrollIntoView = {},
          .syncIndexFromSelection = [this] { return m_selected; },
      }
  );
  m_rovingNav.bindFocusArea(m_focusArea);
  setRowGrouping(true);
}

std::size_t Segmented::addOption(std::string_view label) { return addOption(label, std::string_view{}); }

std::size_t Segmented::addOption(std::string_view label, std::string_view glyph) {
  const std::size_t index = m_buttons.size();
  auto btn = makeSegmentButton(label, glyph, index);
  Button* raw = btn.get();
  m_buttons.push_back(raw);
  m_rovingNav.registerItem(raw, [this, index] { setSelectedIndex(index); });
  addChild(std::move(btn));
  refreshVariants();
  return index;
}

void Segmented::setSelectedIndex(std::size_t index) {
  if (index >= m_buttons.size() || index == m_selected) {
    return;
  }
  m_selected = index;
  refreshVariants();
  m_rovingNav.notifyExternalSelectionChanged();
  if (m_onChange) {
    m_onChange(index);
  }
}

void Segmented::setFontSize(float size) {
  m_fontSize = size;
  const float fs = effectiveFontSize();
  for (Button* btn : m_buttons) {
    if (btn != nullptr) {
      btn->setFontSize(fs);
      btn->setGlyphSize(fs);
    }
  }
}

void Segmented::setScale(float scale) {
  m_scale = std::max(0.1F, scale);
  setGap(6.0F * m_scale);
  applyOuterStyle();
  const float fs = effectiveFontSize();
  for (Button* btn : m_buttons) {
    if (btn != nullptr) {
      applyButtonMetrics(*btn);
      btn->setFontSize(fs);
      btn->setGlyphSize(fs);
    }
  }
  refreshVariants();
  markLayoutDirty();
}

void Segmented::setCompact(bool compact) {
  if (m_compact == compact) {
    return;
  }
  m_compact = compact;
  for (Button* btn : m_buttons) {
    if (btn != nullptr) {
      applyButtonMetrics(*btn);
    }
  }
  markLayoutDirty();
}

void Segmented::setPadding(float padding) {
  m_outerPadding = padding;
  Flex::setPadding(padding);
}

void Segmented::setPadding(float vertical, float horizontal) {
  m_outerPadding = vertical;
  Flex::setPadding(vertical, horizontal);
}

void Segmented::setPadding(float top, float right, float bottom, float left) {
  m_outerPadding = top;
  Flex::setPadding(top, right, bottom, left);
}

void Segmented::setOptionTooltip(std::size_t index, std::string_view text) {
  if (index < m_buttons.size() && m_buttons[index] != nullptr) {
    m_buttons[index]->setTooltip(text);
  }
}

void Segmented::clearOptions() {
  for (Button* btn : m_buttons) {
    if (btn != nullptr) {
      removeChild(btn);
    }
  }
  m_buttons.clear();
  m_rovingNav.clearItems();
  m_selected = 0;
  markLayoutDirty();
}

void Segmented::setOnChange(std::function<void(std::size_t)> callback) { m_onChange = std::move(callback); }

void Segmented::setSurfaceOpacity(float opacity) {
  const float clamped = std::clamp(opacity, 0.0F, 1.0F);
  if (m_surfaceOpacity == clamped) {
    return;
  }
  m_surfaceOpacity = clamped;
  applyOuterStyle();
}

void Segmented::setSurfaceRole(ColorRole role) {
  if (m_surfaceRole == role) {
    return;
  }
  m_surfaceRole = role;
  applyOuterStyle();
}

void Segmented::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  for (Button* btn : m_buttons) {
    if (btn != nullptr) {
      btn->setEnabled(enabled);
    }
  }
  setOpacity(enabled ? 1.0F : 0.55F);
}

std::unique_ptr<Button>
Segmented::makeSegmentButton(std::string_view label, std::string_view glyph, std::size_t index) {
  auto btn = std::make_unique<Button>();
  if (!glyph.empty()) {
    btn->setGlyph(glyph);
    btn->setGlyphSize(effectiveFontSize());
  }
  if (!label.empty()) {
    btn->setText(label);
    btn->setFontSize(effectiveFontSize());
  }
  applyButtonMetrics(*btn);
  btn->setOnClick([this, index] { setSelectedIndex(index); });
  btn->setTabStop(false);
  btn->setFlexGrow(m_equalSegmentWidths ? 1.0F : 0.0F);
  btn->setContentAlign(ButtonContentAlign::Center);
  btn->setEnabled(m_enabled);
  return btn;
}

void Segmented::applyButtonMetrics(Button& button) const {
  if (m_compact) {
    button.setMinHeight(Style::controlHeightSm * m_scale);
    button.setPadding(Style::spaceXs * m_scale, Style::spaceSm * m_scale);
    return;
  }

  button.setMinHeight(Style::controlHeight * m_scale);
  button.setPadding(Style::spaceXs * m_scale, Style::spaceMd * m_scale);
}

void Segmented::setEqualSegmentWidths(bool equalWidths) {
  if (m_equalSegmentWidths == equalWidths) {
    return;
  }
  m_equalSegmentWidths = equalWidths;
  for (Button* b : m_buttons) {
    if (b != nullptr) {
      b->setFlexGrow(m_equalSegmentWidths ? 1.0F : 0.0F);
    }
  }
  markLayoutDirty();
}

void Segmented::refreshVariants() {
  const std::size_t n = m_buttons.size();
  for (std::size_t i = 0; i < n; ++i) {
    if (m_buttons[i] == nullptr) {
      continue;
    }
    m_buttons[i]->setVariant(i == m_selected ? ButtonVariant::TabActive : ButtonVariant::Tab);
  }
}

void Segmented::applyOuterStyle() {
  Flex::setPadding(0.0F);
  clearFill();
  clearBorder();
}

void Segmented::doLayout(Renderer& renderer) {
  Flex::doLayout(renderer);
  m_rovingNav.layoutOverlay(width(), height());
}

float Segmented::effectiveFontSize() const noexcept {
  return (m_fontSize > 0.0F ? m_fontSize : Style::fontSizeBody) * m_scale;
}
