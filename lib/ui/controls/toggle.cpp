#include "ui/controls/toggle.h"

#include "core/input/keybind_matcher.h"
#include "render/animation/animation_manager.h"
#include "render/core/color.h"
#include "render/core/render_styles.h"
#include "render/scene/input_area.h"
#include "render/scene/rect_node.h"
#include "ui/controls/glyph.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <algorithm>
#include <cmath>
#include <memory>

namespace {
  constexpr float haloAlphaHover = 0.16F;
  constexpr float haloAlphaPress = 0.24F;
} // namespace

Toggle::Toggle() {
  setAlign(FlexAlign::Center);
  setDirection(FlexDirection::Horizontal);
  setMirrorInRtl(false);
  setBorder(colorSpecFromRole(ColorRole::SurfaceContainerHighest), Style::borderWidth);

  auto halo = std::make_unique<RectNode>();
  halo->setParticipatesInLayout(false);
  m_halo = static_cast<RectNode*>(addChild(std::move(halo)));

  auto thumb = std::make_unique<RectNode>();
  thumb->setParticipatesInLayout(false);
  m_thumb = static_cast<RectNode*>(addChild(std::move(thumb)));

  auto checkGlyph = std::make_unique<Glyph>();
  checkGlyph->setGlyph("check");
  m_checkGlyph = static_cast<Glyph*>(m_thumb->addChild(std::move(checkGlyph)));

  auto area = std::make_unique<InputArea>();
  area->setOnEnter([this](const InputArea::PointerData& /*data*/) {
    updateHaloState();
  });
  area->setOnLeave([this] {
    updateHaloState();
  });
  area->setOnPress([this](const InputArea::PointerData& /*data*/) {
    updateHaloState();
  });
  area->setOnClick([this](const InputArea::PointerData& /*data*/) {
    if (!m_enabled) {
      return;
    }
    activateFromInput();
  });
  area->setFocusable(true);
  area->setOnFocusGain([this] { applyState(); });
  area->setOnFocusLoss([this] { applyState(); });
  area->setOnKeyDown([this](const InputArea::KeyData& key) {
    if (!key.pressed || !m_enabled) {
      return;
    }
    if (KeybindMatcher::matches(KeybindAction::Validate, key.sym, key.modifiers)) {
      activateFromInput();
    }
  });
  m_inputArea = static_cast<InputArea*>(addChild(std::move(area)));
  m_inputArea->setParticipatesInLayout(false);
  m_inputArea->setZIndex(1);

  applySize();
  applyState();
  m_paletteConn = paletteChanged().connect([this] { applyAnimatedState(m_animationProgress); });
}

Toggle::~Toggle() {
  if (m_animId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_animId);
    m_animId = 0;
  }
  if (m_haloAnimId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_haloAnimId);
    m_haloAnimId = 0;
  }
}

void Toggle::setChecked(bool checked) {
  if (m_checked == checked) {
    return;
  }
  m_checked = checked;

  if (animationManager() != nullptr) {
    if (m_animId != 0) {
      animationManager()->cancel(m_animId);
    }
    float from = m_checked ? 0.0F : 1.0F;
    float to = m_checked ? 1.0F : 0.0F;
    m_animId = animationManager()->animate(
        from, to, Style::animFast, Easing::EaseOutCubic, [this](float t) { applyAnimatedState(t); },
        [this] { m_animId = 0; }, this
    );
    markLayoutDirty();
  } else {
    applyState();
  }
}

void Toggle::setCheckedImmediate(bool checked) {
  if (m_checked == checked) {
    return;
  }
  m_checked = checked;
  if (m_animId != 0 && animationManager() != nullptr) {
    animationManager()->cancel(m_animId);
    m_animId = 0;
  }
  applyState();
}

void Toggle::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (m_inputArea != nullptr) {
    m_inputArea->setEnabled(enabled);
  }
  applyState();
}

void Toggle::setToggleSize(ToggleSize size) {
  if (m_size == size) {
    return;
  }
  m_size = size;
  applySize();
  applyState();
}

void Toggle::setScale(float scale) {
  m_scale = std::max(0.1F, scale);
  applySize();
  applyState();
  markLayoutDirty();
}

void Toggle::setOnChange(std::function<void(bool)> callback) { m_onChange = std::move(callback); }

void Toggle::setTabFocusKey(std::string key) {
  if (m_inputArea != nullptr) {
    m_inputArea->setTabFocusKey(std::move(key));
  }
}

void Toggle::activateFromInput() {
  const bool next = !m_checked;
  setChecked(next);
  if (m_onChange) {
    m_onChange(next);
  }
}

bool Toggle::hovered() const noexcept { return m_inputArea != nullptr && m_inputArea->hovered(); }

bool Toggle::pressed() const noexcept { return m_inputArea != nullptr && m_inputArea->pressed(); }

void Toggle::doLayout(Renderer& renderer) {
  Flex::doLayout(renderer);

  if (m_checkGlyph != nullptr) {
    m_checkGlyph->setGlyphSize(Style::toggleIconSize * m_scale);
    m_checkGlyph->measure(renderer);
  }

  if (m_inputArea != nullptr) {
    m_inputArea->setPosition(0.0F, 0.0F);
    m_inputArea->setFrameSize(width(), height());
  }

  applyAnimatedState(m_animationProgress);
}

LayoutSize Toggle::doMeasure(Renderer& renderer, const LayoutConstraints& constraints) {
  return measureByLayout(renderer, constraints);
}

void Toggle::doArrange(Renderer& renderer, const LayoutRect& rect) { arrangeByLayout(renderer, rect); }

void Toggle::updateHaloState() {
  if (animationManager() == nullptr || !m_enabled) {
    m_haloProgress = 0.0F;
    applyAnimatedState(m_animationProgress);
    return;
  }
  if (m_haloAnimId != 0) {
    animationManager()->cancel(m_haloAnimId);
    m_haloAnimId = 0;
  }
  const float target = (hovered() || pressed()) ? 1.0F : 0.0F;
  if (std::abs(m_haloProgress - target) < 0.001F) {
    m_haloProgress = target;
    applyAnimatedState(m_animationProgress);
    return;
  }
  const float from = m_haloProgress;
  m_haloAnimId = animationManager()->animate(
      from, target, Style::animFast, Easing::EaseOutCubic,
      [this](float s) {
        m_haloProgress = s;
        applyAnimatedState(m_animationProgress);
      },
      [this] { m_haloAnimId = 0; },
      this
  );
  applyAnimatedState(m_animationProgress);
}

void Toggle::applySize() {
  m_thumbSize = Style::toggleThumbSize * m_scale;
  setSize(Style::toggleTrackWidth * m_scale, Style::toggleTrackHeight * m_scale);
  setRadius(Style::toggleTrackHeight * 0.5F * m_scale);
}

void Toggle::applyState() { applyAnimatedState(m_checked ? 1.0F : 0.0F); }

void Toggle::applyAnimatedState(float t) {
  m_animationProgress = t;
  if (!m_enabled) {
    m_haloProgress = 0.0F;
  }

  const Color trackColor = lerpColor(colorForRole(ColorRole::OutlineVariant), colorForRole(ColorRole::Primary), t);
  const Color thumbColor =
      lerpColor(colorForRole(ColorRole::OnSurfaceVariant), colorForRole(ColorRole::OnPrimary), t);

  const float off = Style::toggleThumbOff * m_scale;
  const float on = Style::toggleThumbSize * m_scale;
  const float d = std::lerp(off, on, t);

  const float p = Style::rtl() ? 1.0F - t : t;
  const float centerX = std::lerp(16.0F, 36.0F, p) * m_scale;
  const float thumbX = centerX - d * 0.5F;
  const float thumbY = (Style::toggleTrackHeight * m_scale - d) * 0.5F;

  setFill(trackColor);
  setBorder(fixedColorSpec(lerpColor(colorForRole(ColorRole::OutlineVariant), colorForRole(ColorRole::Primary), t)), Style::borderWidth);

  m_thumb->setPosition(thumbX, thumbY);
  auto thumbStyle = m_thumb->style();
  thumbStyle.fillMode = FillMode::Solid;
  thumbStyle.radius = d * 0.5F;
  thumbStyle.softness = 1.0F;
  thumbStyle.borderWidth = 0.0F;
  thumbStyle.fill = thumbColor;
  m_thumb->setStyle(thumbStyle);
  m_thumb->setFrameSize(d, d);

  const float haloDiam = std::lerp(d, Style::toggleHaloSize * m_scale, m_haloProgress);
  const float haloX = centerX - haloDiam * 0.5F;
  const float haloY = (Style::toggleTrackHeight * m_scale - haloDiam) * 0.5F;
  m_halo->setPosition(haloX, haloY);
  auto haloStyle = m_halo->style();
  haloStyle.fillMode = FillMode::Solid;
  haloStyle.radius = haloDiam * 0.5F;
  haloStyle.softness = 1.0F;
  haloStyle.borderWidth = 0.0F;
  haloStyle.fill = lerpColor(colorForRole(ColorRole::OnSurface), colorForRole(ColorRole::Primary), t);
  m_halo->setStyle(haloStyle);
  m_halo->setFrameSize(haloDiam, haloDiam);
  const float haloAlpha = (!m_enabled) ? 0.0F : (pressed() ? haloAlphaPress : haloAlphaHover);
  m_halo->setOpacity(m_haloProgress * haloAlpha);

  if (m_checkGlyph != nullptr) {
    if (m_checkGlyph->width() > 0.0F && m_checkGlyph->height() > 0.0F) {
      m_checkGlyph->setPosition(
          std::round((d - m_checkGlyph->width()) * 0.5F),
          std::round((d - m_checkGlyph->height()) * 0.5F)
      );
    }
    m_checkGlyph->setColor(colorSpecFromRole(ColorRole::OnPrimaryContainer));
    m_checkGlyph->setVisible(m_checked);
    m_checkGlyph->setOpacity(m_checked ? 1.0F : 0.0F);
  }

  const float rightPad = Style::toggleTrackWidth * m_scale - thumbX - d;
  setPadding(0.0F, rightPad, 0.0F, thumbX);

  if (m_enabled) {
    setOpacity(1.0F);
  } else {
    setOpacity(0.55F);
  }
}
