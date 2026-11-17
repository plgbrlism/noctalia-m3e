#pragma once

#include "ui/controls/flex.h"
#include "ui/style.h"

#include <functional>

class Box;
class InputArea;

class Slider : public Flex {
public:
  Slider();
  ~Slider() override;

  void setRange(double minValue, double maxValue);
  void setStep(double step);
  void setValue(double value);
  void setEnabled(bool enabled);
  void setTrackHeight(float height);
  void setThumbSize(float size);
  void setControlHeight(float height);
  void setScale(float scale);
  void setWheelAdjustEnabled(bool enabled);
  void setOnValueChanged(std::function<void(double)> callback);
  void setOnRangeChanged(std::function<void(double, double)> callback);
  void setOnDragEnd(std::function<void()> callback);
  void setRangeEnabled(bool enabled);
  void setValue2(double value);
  void setRangeValues(double low, double high);

  double value() const noexcept { return m_value; }
  double value2() const noexcept { return m_value2; }
  double minValue() const noexcept { return m_min; }
  double maxValue() const noexcept { return m_max; }
  float scale() const noexcept { return m_scale; }
  bool enabled() const noexcept { return m_enabled; }
  bool rangeEnabled() const noexcept { return m_isRange; }
  bool dragging() const noexcept;
  bool pressed() const noexcept { return m_pressed; }

private:
  void doLayout(Renderer& renderer) override;
  LayoutSize doMeasure(Renderer& renderer, const LayoutConstraints& constraints) override;
  void doArrange(Renderer& renderer, const LayoutRect& rect) override;
  void updateFromLocalX(float x);
  void updateGeometry();
  void applyVisualState();
  void updateSquishState();
  float normalizedValue() const noexcept;
  double snapped(double value) const noexcept;

  Box* m_inactiveTrackLeft = nullptr;
  Box* m_inactiveTrackRight = nullptr;
  Box* m_activeTrack = nullptr;
  Box* m_handle = nullptr;
  Box* m_handle2 = nullptr;
  Box* m_dotLeft = nullptr;
  Box* m_dotRight = nullptr;
  InputArea* m_inputArea = nullptr;

  std::function<void(double)> m_onValueChanged;
  std::function<void(double, double)> m_onRangeChanged;
  std::function<void()> m_onDragEnd;

  double m_min = 0.0;
  double m_max = 100.0;
  double m_step = 1.0;
  double m_value = 50.0;
  double m_value2 = 50.0;
  bool m_enabled = true;
  bool m_wheelAdjustEnabled = true;
  bool m_isRange = false;
  float m_scale = 1.0F;
  float m_trackHeight = Style::sliderTrackHeight;
  float m_thumbSizePx = Style::sliderHandleWidth;
  float m_controlHeightPx = Style::sliderHandleHeight;
  float m_handleSquish = 0.0F;
  std::uint32_t m_squishAnimId = 0;
  int m_activeHandle = 1;
  bool m_pressed = false;
};
