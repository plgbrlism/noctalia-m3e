#pragma once
#include "config/config_types.h"
#include <algorithm>
#include <cstdint>

namespace noctalia::shell::panel::reveal {
  // Order matches AttachedRevealDirection (Down, Up, Right, Left) so
  // static_cast is safe. Do not reorder without updating panel_manager.cpp.
  enum class RevealDirection : std::uint8_t { Down, Up, Right, Left };

  struct RevealTransform {
    float scaleX = 1.0F;
    float scaleY = 1.0F;
    float offsetX = 0.0F;
    float offsetY = 0.0F;
  };

  inline constexpr float kFluidTravel = 24.0F;   // logical px
  inline constexpr float kFluidSquash = 0.02F;   // perpendicular swell

  inline RevealTransform chromeTransform(float progress, PanelMotion variant,
                                          RevealDirection dir, bool attached) noexcept {
    const float p = std::clamp(progress, 0.0F, 1.0F);
    switch (variant) {
    case PanelMotion::Standard:
      return {};
    case PanelMotion::Depth:
      return {0.92F + 0.08F * p, 0.92F + 0.08F * p, 0.0F, 0.0F};
    case PanelMotion::Fluid:
      break;
    }
    // Fluid below. Reduce motion: identity.
    // (Caller is responsible for not invoking when reduce snaps — see
    // panel_manager.cpp. Belt-and-suspenders: if progress is exactly 1
    // we're already identity by construction.)
    const float s = 1.0F - p;
    RevealTransform t;
    const float swell = 1.0F + kFluidSquash * s;
    switch (dir) {
    case RevealDirection::Down:
      t.scaleX = swell;
      if (!attached) t.offsetY = -kFluidTravel * s;
      break;
    case RevealDirection::Up:
      t.scaleX = swell;
      if (!attached) t.offsetY = +kFluidTravel * s;
      break;
    case RevealDirection::Right:
      t.scaleY = swell;
      if (!attached) t.offsetX = -kFluidTravel * s;
      break;
    case RevealDirection::Left:
      t.scaleY = swell;
      if (!attached) t.offsetX = +kFluidTravel * s;
      break;
    }
    return t;
  }
} // namespace noctalia::shell::panel::reveal
