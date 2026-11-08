#pragma once

#include "render/animation/animation.h"
#include "render/animation/motion_scheme.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

struct SpringState {
  float x0 = 0.0F;
  float v0 = 0.0F;
  float x = 0.0F;
  float v = 0.0F;
  float target = 0.0F;
  float dampingRatio = 1.0F;
  float stiffness = 0.0F;
  float elapsed = 0.0F;
  float eps_x = 0.5F;
  float eps_v = 1.0F;
  std::function<void(float)> setter;
  const void* owner = nullptr;
  bool finished = false;
};

class AnimationManager {
public:
  using Id = std::uint32_t;

  AnimationManager();
  ~AnimationManager();

  AnimationManager(const AnimationManager&) = delete;
  AnimationManager& operator=(const AnimationManager&) = delete;
  AnimationManager(AnimationManager&&) = delete;
  AnimationManager& operator=(AnimationManager&&) = delete;

  Id animate(
      float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
      std::function<void()> onComplete = {}, const void* owner = nullptr
  );
  // Real elapsed-time driver: ignores global motion enable/speed. Use for timeouts,
  // and for visual effects with a deliberate fixed duration that must stay independent
  // of the global animation settings (e.g. the wallpaper crossfade).
  Id animateTimer(
      float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
      std::function<void()> onComplete = {}, const void* owner = nullptr
  );
  void cancel(Id id);
  void cancelAll();
  void reduceMotion();
  // Cancels any animations tagged with the given owner. Called from Node's destructor so that
  // animations holding a raw pointer to a scene node can never outlive their target.
  void cancelForOwner(const void* owner);
  void tick(float deltaMs);
  bool hasActive() const;

  Id animateSpring(
      float initial, float target, SpringSpec spec, std::function<void(float)> setter,
      const void* owner = nullptr, float eps_x = 0.5F, float eps_v = 1.0F
  );
  bool retargetSpring(Id id, float target);
  bool retargetSpringSpec(Id id, SpringSpec spec);
  void tickSprings(float dt);
  bool hasActiveSprings() const;
  std::optional<float> springValue(Id id) const;

private:
  struct Entry {
    Id id = 0;
    const void* owner = nullptr;
    bool respectMotionEnabled = true;
    Animation animation;
  };

  std::vector<Entry> m_animations;
  std::unordered_map<Id, SpringState> m_springs;
  Id m_nextId = 1;

  Id animateInternal(
      float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
      std::function<void()> onComplete, const void* owner, bool scaleDuration, bool respectMotionEnabled
  );
};
