#pragma once

#include "ui/controls/flex.h"
#include "ui/signal.h"

#include <functional>
#include <string>

enum class ToggleSize : std::uint8_t {
  Small,
  Medium,
  Large,
};

class Glyph;
class InputArea;

class Toggle : public Flex {
public:
  Toggle();
  ~Toggle() override;

  void setChecked(bool checked);
  void setCheckedImmediate(bool checked);
  void setEnabled(bool enabled);
  void setToggleSize(ToggleSize size);
  void setScale(float scale);
  void setOnChange(std::function<void(bool)> callback);
  void setTabFocusKey(std::string key);
  bool hovered() const noexcept;
  bool pressed() const noexcept;
  bool checked() const noexcept { return m_checked; }
  bool enabled() const noexcept { return m_enabled; }
  ToggleSize toggleSize() const noexcept { return m_size; }

private:
  void activateFromInput();
  void applySize();
  void applyState();
  void applyAnimatedState(float t);
  void updateHaloState();
  void doLayout(Renderer& renderer) override;
  LayoutSize doMeasure(Renderer& renderer, const LayoutConstraints& constraints) override;
  void doArrange(Renderer& renderer, const LayoutRect& rect) override;

  class RectNode* m_halo = nullptr;
  class RectNode* m_thumb = nullptr;
  Glyph* m_checkGlyph = nullptr;
  InputArea* m_inputArea = nullptr;
  std::uint32_t m_animId = 0;
  std::uint32_t m_haloAnimId = 0;
  std::function<void(bool)> m_onChange;
  ToggleSize m_size = ToggleSize::Medium;
  bool m_checked = false;
  bool m_enabled = true;
  float m_thumbSize = 0.0F;
  float m_scale = 1.0F;
  float m_animationProgress = 0.0F;
  float m_haloProgress = 0.0F; // halo progress, 0 to 1
  Signal<>::ScopedConnection m_paletteConn;
};
