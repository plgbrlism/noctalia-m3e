#pragma once

#include "ui/controls/flex.h"
#include "ui/style.h"

#include <functional>

class Box;
class InputArea;

// Dual-thumb slider expressing a low/high pair on one axis (e.g. activity + critical
// thresholds). The two thumbs cannot cross — the low value is always <= the high value.
class RangeSlider : public Flex {
public:
  RangeSlider();
  ~RangeSlider() override;

  void setScale(float scale);
  void setRange(double minValue, double maxValue);
  void setStep(double step);
  void setLowValue(double value);
  void setHighValue(double value);
  // Sets both endpoints at once (init path) without cross-clamping against stale values.
  void setValues(double low, double high);
  void setEnabled(bool enabled);
  void setTrackHeight(float height);
  void setThumbSize(float size);
  void setControlHeight(float height);
  void setOnLowChanged(std::function<void(double)> callback);
  void setOnHighChanged(std::function<void(double)> callback);
  void setOnDragEnd(std::function<void()> callback);

  double lowValue() const noexcept { return m_low; }
  double highValue() const noexcept { return m_high; }
  double minValue() const noexcept { return m_min; }
  double maxValue() const noexcept { return m_max; }
  float scale() const noexcept { return m_scale; }
  bool enabled() const noexcept { return m_enabled; }
  bool dragging() const noexcept;

private:
  enum class ActiveThumb : std::uint8_t { None, Low, High };

  void doLayout(Renderer& renderer) override;
  LayoutSize doMeasure(Renderer& renderer, const LayoutConstraints& constraints) override;
  void doArrange(Renderer& renderer, const LayoutRect& rect) override;
  void updateFromLocalX(float x);
  void updateGeometry();
  void applyVisualState();
  void updateSquishState();
  float normalized(double value) const noexcept;
  double snapped(double value) const noexcept;
  float thumbCenterX(double value) const noexcept;

  Box* m_inactiveTrackLeft = nullptr;
  Box* m_inactiveTrackRight = nullptr;
  Box* m_activeTrack = nullptr;
  Box* m_lowHandle = nullptr;
  Box* m_highHandle = nullptr;
  Box* m_dotLeft = nullptr;
  Box* m_dotRight = nullptr;
  InputArea* m_inputArea = nullptr;

  std::function<void(double)> m_onLowChanged;
  std::function<void(double)> m_onHighChanged;
  std::function<void()> m_onDragEnd;

  double m_min = 0.0;
  double m_max = 100.0;
  double m_step = 1.0;
  double m_low = 25.0;
  double m_high = 75.0;
  ActiveThumb m_activeThumb = ActiveThumb::None;
  bool m_enabled = true;
  bool m_pressed = false;
  float m_scale = 1.0F;
  float m_handleSquish = 0.0F;
  std::uint32_t m_squishAnimId = 0;
  float m_trackHeight = Style::sliderTrackHeight;
  float m_thumbSizePx = Style::sliderHandleWidth;
  float m_controlHeightPx = Style::sliderHandleHeight;
};
