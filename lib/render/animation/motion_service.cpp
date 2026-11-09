#include "render/animation/motion_service.h"

#include "render/animation/animation_manager.h"

#include <algorithm>
#include <vector>

MotionService& MotionService::instance() {
  static MotionService service;
  return service;
}

void MotionService::registerManager(AnimationManager* manager) {
  if (manager == nullptr) {
    return;
  }
  m_managers.insert(manager);
}

void MotionService::unregisterManager(AnimationManager* manager) {
  if (manager == nullptr) {
    return;
  }
  m_managers.erase(manager);
}

void MotionService::setEnabled(bool enabled) {
  if (m_enabled == enabled) {
    return;
  }
  m_enabled = enabled;
  if (!m_enabled) {
    const std::vector<AnimationManager*> managers(m_managers.begin(), m_managers.end());
    for (auto* manager : managers) {
      if (manager != nullptr && m_managers.contains(manager)) {
        manager->reduceMotion();
      }
    }
  }
}

void MotionService::setSpeed(float speed) { m_speed = std::clamp(speed, 0.05F, 4.0F); }

void MotionService::setMotionConfig(const MotionConfig& config) { m_motionConfig = config; }

const MotionConfig& MotionService::motionConfig() const { return m_motionConfig; }

SpringSpec MotionService::resolve(SpringToken token) const {
  SpringSpec spec = SpringTokenResolver(m_motionConfig).resolve(token);
  if (!m_enabled) {
    spec.dampingRatio = 1.0F;
    spec.stiffness = 1.0e6F;
    return spec;
  }
  if (m_speed != 1.0F && m_speed > 0.0F) {
    const float kMultiplier = std::clamp(m_speed * m_speed, 0.25F, 4.0F);
    spec.stiffness *= kMultiplier;
  }
  return spec;
}
