#include "render/animation/animation_manager.h"

#include "render/animation/motion_service.h"

#include <chrono>
#include <cmath>

namespace {
  constexpr float kReducedMotionDurationMs = 1.0F;
}

AnimationManager::AnimationManager() { MotionService::instance().registerManager(this); }

AnimationManager::~AnimationManager() { MotionService::instance().unregisterManager(this); }

AnimationManager::Id AnimationManager::animate(
    float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
    std::function<void()> onComplete, const void* owner
) {
  return animateInternal(from, to, durationMs, easing, std::move(setter), std::move(onComplete), owner, true, true);
}

AnimationManager::Id AnimationManager::animateTimer(
    float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
    std::function<void()> onComplete, const void* owner
) {
  return animateInternal(from, to, durationMs, easing, std::move(setter), std::move(onComplete), owner, false, false);
}

AnimationManager::Id AnimationManager::animateInternal(
    float from, float to, float durationMs, Easing easing, std::function<void(float)> setter,
    std::function<void()> onComplete, const void* owner, bool scaleDuration, bool respectMotionEnabled
) {
  const auto& motion = MotionService::instance();
  const bool reduceMotion = respectMotionEnabled && !motion.enabled();
  if (reduceMotion) {
    if (setter) {
      setter(to);
    }
    if (!onComplete) {
      return 0;
    }
  }

  float effectiveDurationMs = scaleDuration ? durationMs / motion.speed() : durationMs;
  if (reduceMotion) {
    effectiveDurationMs = kReducedMotionDurationMs;
  }

  if (effectiveDurationMs <= 0.0F) {
    if (setter) {
      setter(to);
    }
    if (onComplete) {
      onComplete();
    }
    return 0;
  }

  Id id = m_nextId++;
  const auto now = std::chrono::steady_clock::now();
  m_animations.push_back(
      Entry{
          .id = id,
          .owner = owner,
          .respectMotionEnabled = respectMotionEnabled,
          .animation = Animation{
              .startValue = reduceMotion ? to : from,
              .endValue = to,
              .durationMs = effectiveDurationMs,
              .startedAt = reduceMotion ? now
                      - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                                              std::chrono::duration<float, std::milli>(kReducedMotionDurationMs)
                      )
                                        : now,
              .easing = easing,
              .setter = std::move(setter),
              .onComplete = std::move(onComplete),
          },
      }
  );
  return id;
}

void AnimationManager::cancel(Id id) {
  std::erase_if(m_animations, [id](const Entry& e) { return e.id == id; });
}

void AnimationManager::cancelAll() { m_animations.clear(); }

void AnimationManager::reduceMotion() {
  const auto now = std::chrono::steady_clock::now();
  for (auto& entry : m_animations) {
    if (!entry.respectMotionEnabled || entry.animation.finished) {
      continue;
    }

    auto& anim = entry.animation;
    if (anim.setter) {
      anim.setter(anim.endValue);
    }
    anim.startValue = anim.endValue;
    anim.durationMs = kReducedMotionDurationMs;
    anim.elapsedMs = 0.0F;
    anim.startedAt = now
        - std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                         std::chrono::duration<float, std::milli>(kReducedMotionDurationMs)
        );
  }
}

void AnimationManager::cancelForOwner(const void* owner) {
  if (owner == nullptr) {
    return;
  }
  std::erase_if(m_animations, [owner](const Entry& e) { return e.owner == owner; });
  std::erase_if(m_springs, [owner](const auto& pair) {
    return pair.second.owner == owner;
  });
}

void AnimationManager::tick(float /*deltaMs*/) {
  // Collect completed callbacks separately — onComplete may call animate()
  // which would push_back and invalidate iterators during iteration.
  std::vector<std::function<void()>> completedCallbacks;

  const auto now = std::chrono::steady_clock::now();

  for (auto& entry : m_animations) {
    auto& anim = entry.animation;
    if (anim.finished) {
      continue;
    }

    // Progress from wall time so a fixed duration stays correct when the compositor
    // delivers sparse `wl_surface.frame` callbacks (common right after cold boot).
    const float wallElapsedMs = std::chrono::duration<float, std::milli>(now - anim.startedAt).count();
    anim.elapsedMs = wallElapsedMs;
    float t = anim.durationMs > 0.0F ? wallElapsedMs / anim.durationMs : 1.0F;

    if (t >= 1.0F) {
      t = 1.0F;
      anim.finished = true;
    }

    float easedT = applyEasing(anim.easing, t);
    float value = anim.startValue + (anim.endValue - anim.startValue) * easedT;

    if (anim.setter) {
      anim.setter(value);
    }

    if (anim.finished && anim.onComplete) {
      completedCallbacks.push_back(std::move(anim.onComplete));
    }
  }

  std::erase_if(m_animations, [](const Entry& e) { return e.animation.finished; });

  for (auto& cb : completedCallbacks) {
    cb();
  }
}

bool AnimationManager::hasActive() const { return !m_animations.empty(); }

AnimationManager::Id AnimationManager::animateSpring(
    float initial, float target, SpringSpec spec, std::function<void(float)> setter,
    std::function<void()> onComplete, const void* owner, float eps_x, float eps_v
) {
  const Id id = m_nextId++;
  m_springs.emplace(
      id, SpringState{
              initial, 0.0F, initial, 0.0F, target, spec.dampingRatio, spec.stiffness, 0.0F,
              eps_x, eps_v, std::move(setter), std::move(onComplete), owner, false,
          }
  );
  requestFrame();
  return id;
}

bool AnimationManager::retargetSpring(Id id, float target) {
  if (id == 0) {
    return false;
  }
  const auto it = m_springs.find(id);
  if (it == m_springs.end()) {
    return false;
  }

  auto& state = it->second;
  const bool wasFinished = state.finished;
  state.x0 = state.x;
  state.v0 = state.v;
  state.elapsed = 0.0F;
  state.target = target;
  state.finished = false;
  if (wasFinished) {
    requestFrame();
  }
  return true;
}

bool AnimationManager::retargetSpringSpec(Id id, SpringSpec spec) {
  if (id == 0) {
    return false;
  }
  const auto it = m_springs.find(id);
  if (it == m_springs.end()) {
    return false;
  }
  auto& state = it->second;
  const bool wasFinished = state.finished;
  state.x0 = state.x;
  state.v0 = state.v;
  state.elapsed = 0.0F;
  state.dampingRatio = spec.dampingRatio;
  state.stiffness = spec.stiffness;
  state.finished = false;
  if (wasFinished) {
    requestFrame();
  }
  return true;
}

void AnimationManager::tickSprings(float dt) {
  constexpr float kZetaEps = 1e-4F;

  if (dt > 0.032F) {
    dt = 0.032F;
  }

  m_frameRequested = false;

  // Snapshot IDs: setter callbacks may call animateSpring() and rehash
  // m_springs, invalidating any iterator held across the callback.
  std::vector<Id> ids;
  ids.reserve(m_springs.size());
  for (const auto& entry : m_springs) {
    ids.push_back(entry.first);
  }

  for (Id id : ids) {
    const auto it = m_springs.find(id);
    if (it == m_springs.end()) {
      continue;
    }
    auto& state = it->second;
    if (state.finished) {
      continue;
    }

    state.elapsed += dt;

    const float w0 = std::sqrt(state.stiffness);
    const float d0 = state.x0 - state.target;
    const float zeta = state.dampingRatio;
    const float t = state.elapsed;

    float d = 0.0F;
    float v = 0.0F;

    if (zeta < 1.0F - kZetaEps) {
      const float wd = w0 * std::sqrt(1.0F - zeta * zeta);
      const float a = zeta * w0;
      const float b = (state.v0 + a * d0) / wd;
      const float decay = std::exp(-a * t);
      const float cosWd = std::cos(wd * t);
      const float sinWd = std::sin(wd * t);
      d = decay * (d0 * cosWd + b * sinWd);
      v = decay * ((-a * d0 + b * wd) * cosWd + (-a * b - d0 * wd) * sinWd);
    } else if (zeta > 1.0F + kZetaEps) {
      const float root = std::sqrt(zeta * zeta - 1.0F);
      const float r1 = w0 * (-zeta + root);
      const float r2 = w0 * (-zeta - root);
      const float c1 = (state.v0 - r2 * d0) / (r1 - r2);
      const float c2 = (r1 * d0 - state.v0) / (r1 - r2);
      const float e1 = std::exp(r1 * t);
      const float e2 = std::exp(r2 * t);
      d = c1 * e1 + c2 * e2;
      v = c1 * r1 * e1 + c2 * r2 * e2;
    } else {
      const float decay = std::exp(-w0 * t);
      const float k = state.v0 + w0 * d0;
      d = decay * (d0 + k * t);
      v = decay * (state.v0 - w0 * k * t);
    }

    state.x = state.target + d;
    state.v = v;

    if (std::abs(state.x - state.target) < state.eps_x && std::abs(state.v) < state.eps_v) {
      state.x = state.target;
      state.v = 0.0F;
      if (state.setter) {
        state.setter(state.target);
      }
      // onComplete fires on every snap (initial settle and every reviving
      // retarget that re-settles). Not cleared — caller may rely on repeat.
      if (state.onComplete) {
        state.onComplete();
      }
      state.finished = true;
    } else if (state.setter) {
      state.setter(state.x);
    }
  }

  if (hasActiveSprings()) {
    requestFrame();
  }
}

bool AnimationManager::hasActiveSprings() const {
  for (const auto& [id, state] : m_springs) {
    (void)id;
    if (!state.finished) {
      return true;
    }
  }
  return false;
}

std::optional<float> AnimationManager::springValue(Id id) const {
  if (id == 0) {
    return std::nullopt;
  }
  const auto it = m_springs.find(id);
  if (it == m_springs.end()) {
    return std::nullopt;
  }
  return it->second.x;
}

bool AnimationManager::hasAnyActive() const { return hasActive() || hasActiveSprings(); }

void AnimationManager::setFrameRequest(FrameRequest request) {
  // m_frameRequested stays true across callback swap — any pending request
  // was already delivered. New callback serves the next frame.
  m_frameRequest = std::move(request);
}

void AnimationManager::requestFrame() {
  if (!m_frameRequested && m_frameRequest) {
    m_frameRequested = true;
    m_frameRequest();
  }
}
