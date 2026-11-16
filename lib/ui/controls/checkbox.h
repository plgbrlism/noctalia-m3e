#pragma once

#include "render/scene/node.h"
#include "ui/palette.h"

#include <cstdint>
#include <functional>
#include <optional>

class Box;
class Glyph;
class InputArea;

class Checkbox : public Node {
public:
  Checkbox();
  ~Checkbox() override;

  void setChecked(bool checked);
  void setIndeterminate(bool indeterminate);
  void setError(bool error);
  void setEnabled(bool enabled);
  void setOnChange(std::function<void(bool)> callback);
  void setScale(float scale);
  void setCheckedColors(std::optional<ColorSpec> fill, std::optional<ColorSpec> border, std::optional<ColorSpec> glyph);
  void setTooltip(std::string_view text);
  void clearTooltip();

  bool checked() const noexcept { return m_checked; }
  bool indeterminate() const noexcept { return m_indeterminate; }
  bool error() const noexcept { return m_error; }
  bool enabled() const noexcept { return m_enabled; }
  bool hovered() const noexcept;
  bool pressed() const noexcept;

private:
  void doLayout(Renderer& renderer) override;
  void applyState();
  void animateStateLayer(float targetAlpha);
  void applyStateLayer(float alpha);
  ColorSpec stateLayerBase() const;

  Box* m_stateLayer = nullptr;
  Box* m_box = nullptr;
  Glyph* m_checkGlyph = nullptr;
  InputArea* m_inputArea = nullptr;
  std::function<void(bool)> m_onChange;
  std::optional<ColorSpec> m_checkedFill;
  std::optional<ColorSpec> m_checkedBorder;
  std::optional<ColorSpec> m_checkedGlyph;
  bool m_checked = false;
  bool m_hovered = false;
  bool m_indeterminate = false;
  bool m_error = false;
  bool m_enabled = true;
  float m_scale = 1.0F;
  float m_stateLayerAlpha = 0.0F;
  std::uint32_t m_animId = 0;
};
