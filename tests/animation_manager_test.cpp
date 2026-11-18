#include "render/animation/animation_manager.h"
#include "render/animation/motion_service.h"
#include "tests/test_check.h"

#include <cmath>
#include <iostream>

namespace {

  bool check(bool cond, const char* msg) {
    if (!cond) {
      std::cerr << "FAIL: " << msg << '\n';
    }
    return cond;
  }

  void resetMotion() {
    auto& motion = MotionService::instance();
    motion.setSpeed(1.0F);
    motion.setEnabled(true);
  }

} // namespace

int main() {
  bool ok = true;
  resetMotion();

  {
    AnimationManager manager;
    MotionService::instance().setEnabled(false);

    float value = 0.0F;
    bool completed = false;
    const auto id = manager.animate(
        0.0F, 10.0F, 200.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed] { completed = true; }
    );

    ok &= check(id != 0, "reduced-motion animation with completion should remain cancellable");
    TEST_NEAR(value, 10.0F, 0.0001F);
    ok &= check(!completed, "reduced-motion completion ran synchronously");

    manager.tick(0.0F);

    ok &= check(completed, "reduced-motion completion did not run on tick");
    ok &= check(!manager.hasActive(), "reduced-motion animation stayed active after completion");
  }

  resetMotion();

  {
    AnimationManager manager;

    float value = 0.0F;
    bool completed = false;
    const auto id = manager.animate(
        0.0F, 10.0F, 1000.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed] { completed = true; }
    );

    ok &= check(id != 0, "normal animation did not start");

    MotionService::instance().setEnabled(false);

    TEST_NEAR(value, 10.0F, 0.0001F);
    ok &= check(!completed, "active animation completion ran synchronously when motion was disabled");
    ok &= check(manager.hasActive(), "active animation did not remain pending for async completion");

    manager.tick(0.0F);

    ok &= check(completed, "active animation completion did not run after reduced-motion snap");
  }

  resetMotion();

  {
    AnimationManager manager;
    MotionService::instance().setEnabled(false);

    float value = -1.0F;
    bool completed = false;
    const auto id = manager.animateTimer(
        1.0F, 0.0F, 1000.0F, Easing::Linear, [&value](float v) { value = v; }, [&completed] { completed = true; }
    );

    ok &= check(id != 0, "timer animation did not start while motion was disabled");
    TEST_NEAR(value, -1.0F, 0.0001F);

    manager.tick(0.0F);

    ok &= check(!completed, "timer animation completed early while motion was disabled");
    ok &= check(manager.hasActive(), "timer animation was removed early while motion was disabled");
    manager.cancel(id);
  }

  resetMotion();

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&requests] { ++requests; });
    MotionService::instance().setEnabled(false);

    float value = -1.0F;
    bool completed = false;
    const auto id = manager.animateSpring(
        0.0F, 5.0F, SpringSpec{0.8F, 380.0F}, [&value](float v) { value = v; }, [&completed] { completed = true; }
    );

    ok &= check(id == 0, "disabled spring should not be registered");
    TEST_NEAR(value, 5.0F, 0.0001F);
    ok &= check(completed, "disabled spring completion did not run synchronously");
    ok &= check(!manager.hasActiveSprings(), "disabled spring stayed active");
    ok &= check(requests == 0, "disabled spring requested a frame");

    manager.tickSprings(0.016F);
    ok &= check(requests == 0, "disabled spring requested a frame on tick");
  }

  resetMotion();

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&requests] { ++requests; });

    float value = 0.0F;
    bool completed = false;
    const auto id = manager.animateSpring(
        0.0F, 10.0F, SpringSpec{0.8F, 380.0F}, [&value](float v) { value = v; }, [&completed] { completed = true; }
    );
    ok &= check(id != 0, "in-flight spring did not start");
    ok &= check(requests == 1, "in-flight spring did not request a frame");

    MotionService::instance().setEnabled(false);

    TEST_NEAR(value, 10.0F, 0.0001F);
    ok &= check(completed, "in-flight spring completion did not run on disable");
    ok &= check(!manager.hasActiveSprings(), "in-flight spring stayed active after disable");
    ok &= check(!manager.retargetSpring(id, 5.0F), "disabled spring retarget should fail");

    requests = 0;
    manager.tickSprings(0.016F);
    ok &= check(requests == 0, "snapped spring requested another frame");
  }

  resetMotion();
  return ok ? 0 : 1;
}
