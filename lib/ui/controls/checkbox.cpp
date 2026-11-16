#include "ui/controls/checkbox.h"

#include "core/input/keybind_matcher.h"
#include "render/animation/animation_manager.h"
#include "render/scene/input_area.h"
#include "ui/controls/box.h"
#include "ui/controls/glyph.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <algorithm>
#include <cmath>
#include <memory>

Checkbox::Checkbox() {
  auto stateLayer = std::make_unique<Box>();
  stateLayer->setFill(clearColorSpec());
  m_stateLayer = static_cast<Box*>(addChild(std::move(stateLayer)));

  auto box = std::make_unique<Box>();
  m_box = static_cast<Box*>(addChild(std::move(box)));

  auto checkGlyph = std::make_unique<Glyph>();
  checkGlyph->setGlyph("check");
  m_checkGlyph = static_cast<Glyph*>(addChild(std::move(checkGlyph)));

  auto area = std::make_unique<InputArea>();
  area->setFocusable(true);
  area->setOnEnter([this](const InputArea::PointerData& /*data*/) {
    if (!m_hovered) {
      m_hovered = true;
      applyState();
    }
  });
  area->setOnLeave([this] {
    if (m_hovered) {
      m_hovered = false;
      applyState();
    }
  });
  area->setOnPress([this](const InputArea::PointerData& /*data*/) { applyState(); });
  area->setOnFocusGain([this] { applyState(); });
  area->setOnFocusLoss([this] { applyState(); });
  area->setOnKeyDown([this](const InputArea::KeyData& key) {
    if (!key.pressed || !m_enabled) {
      return;
    }
    if (!KeybindMatcher::matches(KeybindAction::Validate, key.sym, key.modifiers)) {
      return;
    }
    const bool next = !m_checked;
    m_checked = next;
    m_indeterminate = false;
    m_checkGlyph->setGlyph("check");
    applyState();
    if (m_onChange) {
      m_onChange(next);
    }
  });
  area->setOnClick([this](const InputArea::PointerData& /*data*/) {
    if (!m_enabled) {
      return;
    }
    const bool next = !m_checked;
    m_checked = next;
    m_indeterminate = false;
    m_checkGlyph->setGlyph("check");
    applyState();
    if (m_onChange) {
      m_onChange(next);
    }
  });
  m_inputArea = static_cast<InputArea*>(addChild(std::move(area)));

  applyState();
}

Checkbox::~Checkbox() {
  if (m_animId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_animId);
    m_animId = 0;
  }
}

void Checkbox::setChecked(bool checked) {
  if (m_checked == checked) {
    return;
  }
  m_checked = checked;
  m_indeterminate = false;
  m_checkGlyph->setGlyph("check");
  applyState();
}

void Checkbox::setIndeterminate(bool indeterminate) {
  if (m_indeterminate == indeterminate) {
    return;
  }
  m_indeterminate = indeterminate;
  m_checkGlyph->setGlyph(indeterminate ? "minus" : "check");
  applyState();
}

void Checkbox::setError(bool error) {
  if (m_error == error) {
    return;
  }
  m_error = error;
  applyState();
}

void Checkbox::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (m_inputArea != nullptr) {
    m_inputArea->setEnabled(enabled);
  }
  applyState();
}

void Checkbox::setOnChange(std::function<void(bool)> callback) { m_onChange = std::move(callback); }

void Checkbox::setScale(float scale) {
  m_scale = std::max(0.1F, scale);
  applyState();
  markLayoutDirty();
}

void Checkbox::setCheckedColors(
    std::optional<ColorSpec> fill, std::optional<ColorSpec> border, std::optional<ColorSpec> glyph
) {
  m_checkedFill = fill;
  m_checkedBorder = border;
  m_checkedGlyph = glyph;
  applyState();
}

void Checkbox::setTooltip(std::string_view text) {
  if (m_inputArea == nullptr) {
    return;
  }
  if (text.empty()) {
    m_inputArea->clearTooltip();
  } else {
    m_inputArea->setTooltip(std::string(text));
  }
}

void Checkbox::clearTooltip() {
  if (m_inputArea != nullptr) {
    m_inputArea->clearTooltip();
  }
}

bool Checkbox::hovered() const noexcept { return m_hovered; }

bool Checkbox::pressed() const noexcept { return m_inputArea != nullptr && m_inputArea->pressed(); }

void Checkbox::doLayout(Renderer& renderer) {
  const float targetSize = Style::checkboxTargetSize * m_scale;
  const float stateSize = Style::checkboxStateLayerSize * m_scale;
  const float containerSize = Style::checkboxContainerSize * m_scale;
  const float radius = std::min(Style::checkboxCornerRadius * m_scale, containerSize * 0.5F);
  const float stateInset = (targetSize - stateSize) * 0.5F;
  const float containerInset = (targetSize - containerSize) * 0.5F;

  setSize(targetSize, targetSize);

  if (m_stateLayer != nullptr) {
    m_stateLayer->setPosition(stateInset, stateInset);
    m_stateLayer->setFrameSize(stateSize, stateSize);
    m_stateLayer->setRadius(stateSize * 0.5F);
  }

  if (m_box != nullptr) {
    m_box->setPosition(containerInset, containerInset);
    m_box->setFrameSize(containerSize, containerSize);
    m_box->setRadius(radius);
  }

  if (m_checkGlyph != nullptr) {
    m_checkGlyph->setGlyphSize(Style::checkboxIconSize * m_scale);
    m_checkGlyph->measure(renderer);
    m_checkGlyph->setPosition(
        std::round(containerInset + (containerSize - m_checkGlyph->width()) * 0.5F),
        std::round(containerInset + (containerSize - m_checkGlyph->height()) * 0.5F)
    );
  }

  if (m_inputArea != nullptr) {
    m_inputArea->setPosition(0.0F, 0.0F);
    m_inputArea->setFrameSize(targetSize, targetSize);
  }
}

void Checkbox::applyState() {
  if (m_box == nullptr || m_checkGlyph == nullptr) {
    return;
  }

  const bool active = m_checked || m_indeterminate;

  ColorSpec fill = clearColorSpec();
  ColorSpec border = colorSpecFromRole(m_error ? ColorRole::Error : ColorRole::OnSurfaceVariant);
  ColorSpec glyph = colorSpecFromRole(ColorRole::OnPrimary);
  float borderWidth = Style::checkboxOutlineWidth * m_scale;

  if (active) {
    fill = m_error ? colorSpecFromRole(ColorRole::Error)
                   : m_checkedFill.value_or(colorSpecFromRole(ColorRole::Primary));
    glyph = m_error ? colorSpecFromRole(ColorRole::OnError)
                    : m_checkedGlyph.value_or(colorSpecFromRole(ColorRole::OnPrimary));
    border = m_checkedBorder.value_or(clearColorSpec());
    borderWidth = m_checkedBorder.has_value() ? Style::checkboxOutlineWidth * m_scale : 0.0F;
  }

  m_box->setFill(fill);
  if (borderWidth > 0.0F) {
    m_box->setBorder(border, borderWidth);
  } else {
    m_box->clearBorder();
  }

  m_checkGlyph->setColor(glyph);
  m_checkGlyph->setVisible(active);

  setOpacity(m_enabled ? 1.0F : 0.55F);

  float targetAlpha = 0.0F;
  if (m_enabled) {
    if (pressed()) {
      targetAlpha = Style::checkboxStateLayerPressedAlpha;
    } else if (hovered()) {
      targetAlpha = Style::checkboxStateLayerHoverAlpha;
    }
  }

  animateStateLayer(targetAlpha);
}

ColorSpec Checkbox::stateLayerBase() const {
  if (m_error) {
    return colorSpecFromRole(ColorRole::Error);
  }
  if (m_checked || m_indeterminate) {
    return colorSpecFromRole(ColorRole::Primary);
  }
  return colorSpecFromRole(ColorRole::OnSurface);
}

void Checkbox::applyStateLayer(float alpha) {
  if (m_stateLayer == nullptr) {
    return;
  }
  m_stateLayer->setFill(scaleAlpha(stateLayerBase(), alpha));
  markPaintDirty();
}

void Checkbox::animateStateLayer(float targetAlpha) {
  if (m_stateLayer == nullptr) {
    return;
  }
  if (m_stateLayerAlpha == targetAlpha) {
    applyStateLayer(targetAlpha);
    return;
  }
  if (animationManager() == nullptr) {
    m_stateLayerAlpha = targetAlpha;
    applyStateLayer(targetAlpha);
    return;
  }

  if (m_animId != 0) {
    animationManager()->cancel(m_animId);
    m_animId = 0;
  }

  const float from = m_stateLayerAlpha;
  m_animId = animationManager()->animate(
      from, targetAlpha, Style::animFast, Easing::EaseOutCubic,
      [this](float alpha) {
        m_stateLayerAlpha = alpha;
        applyStateLayer(alpha);
      },
      [this] {
        m_animId = 0;
        if (m_stateLayerAlpha != 0.0F
            && m_stateLayerAlpha != Style::checkboxStateLayerHoverAlpha
            && m_stateLayerAlpha != Style::checkboxStateLayerPressedAlpha) {
          applyState();
        }
      }
  );
  markPaintDirty();
}
