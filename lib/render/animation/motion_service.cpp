#include "render/animation/motion_service.h"

#include "render/animation/animation_manager.h"
#include "ui/style.h"

#include <algorithm>
#include <vector>

MotionService& MotionService::instance() {
  static MotionService service;
  return service;
}

namespace {

  struct PresetOverride {
    MotionScheme scheme;
    SpringToken spatialToken;
    SpringToken effectsToken;
  };

  PresetOverride presetOverrideFor(Style::MotionPreset p) noexcept {
    switch (p) {
    case Style::MotionPreset::Smooth:
      // Standard scheme default = 0.9/700 — minimal overshoot
      return {MotionScheme::Standard,
              SpringToken::SpatialDefault,
              SpringToken::EffectsDefault};
    case Style::MotionPreset::Balanced:
      // Expressive scheme default = 0.8/380 — controlled bounce
      return {MotionScheme::Expressive,
              SpringToken::SpatialDefault,
              SpringToken::EffectsDefault};
    case Style::MotionPreset::Playful:
      // Expressive scheme fast = 0.6/800 — snappy overshoot
      return {MotionScheme::Expressive,
              SpringToken::SpatialFast,
              SpringToken::EffectsFast};
    }
    return {MotionScheme::Expressive, SpringToken::SpatialDefault, SpringToken::EffectsDefault};
  }

  bool isEffectsToken(SpringToken t) noexcept {
    return t == SpringToken::EffectsFast
        || t == SpringToken::EffectsDefault
        || t == SpringToken::EffectsSlow;
  }

} // namespace

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

void MotionService::setReduceMotion(bool reduce) {
  m_reduceMotion = reduce;
}

bool MotionService::reduceMotion() const {
  return m_reduceMotion;
}

SpringSpec MotionService::resolve(SpringToken token) const {
  SpringSpec spec = SpringTokenResolver(m_motionConfig).resolve(token);
  if (!m_enabled) {
    spec.dampingRatio = 1.0F;
    spec.stiffness = 1.0e6F;
    return spec;
  }
  if (Style::designSystem() == Style::DesignSystem::Expressive) {
    const PresetOverride ov = presetOverrideFor(Style::motionPreset());
    MotionConfig effectiveConfig = m_motionConfig;
    effectiveConfig.scheme = ov.scheme;
    const SpringToken effectiveToken =
        isEffectsToken(token) ? ov.effectsToken : ov.spatialToken;
    spec = SpringTokenResolver(effectiveConfig).resolve(effectiveToken);
  }
  if (m_reduceMotion && !isEffectsToken(token)) {
    // Reduce motion: force zeta=1.0 for spatial, keep stiffness
    spec.dampingRatio = 1.0F;
  }
  if (m_speed != 1.0F && m_speed > 0.0F) {
    const float kMultiplier = std::clamp(m_speed * m_speed, 0.25F, 4.0F);
    spec.stiffness *= kMultiplier;
  }
  return spec;
}
