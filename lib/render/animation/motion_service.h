#pragma once

#include "render/animation/motion_scheme.h"

#include <cstdint>
#include <unordered_set>

class AnimationManager;

class MotionService {
public:
  static MotionService& instance();

  MotionService(const MotionService&) = delete;
  MotionService& operator=(const MotionService&) = delete;

  void registerManager(AnimationManager* manager);
  void unregisterManager(AnimationManager* manager);

  void setEnabled(bool enabled);
  void setSpeed(float speed);

  void setMotionConfig(const MotionConfig& config);
  const MotionConfig& motionConfig() const;
  void setReduceMotion(bool reduce);
  bool reduceMotion() const;
  SpringSpec resolve(SpringToken token) const;

  bool enabled() const noexcept { return m_enabled; }
  float speed() const noexcept { return m_speed; }

private:
  MotionService() = default;

  bool m_enabled = true;
  bool m_reduceMotion = false;
  float m_speed = 1.0F;
  MotionConfig m_motionConfig;
  std::unordered_set<AnimationManager*> m_managers;
};
