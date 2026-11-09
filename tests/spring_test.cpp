#include "render/animation/animation_manager.h"
#include "tests/test_check.h"

#include <cmath>

namespace {

  constexpr float kDt = 0.016F;
  constexpr float kTol = 1e-4F;

} // namespace

int main() {
  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    float previous = 0.0F;
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
      const float x = *manager.springValue(id);
      TEST_CHECK(x >= previous - kTol);
      TEST_CHECK(x <= 1.0F + kTol);
      previous = x;
    }

    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(id) == 1.0F);
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    float maxX = -1.0F;
    int frames = 0;
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
      const float x = *manager.springValue(id);
      if (x > maxX) {
        maxX = x;
      }
      ++frames;
    }

    TEST_CHECK(maxX > 1.001F);
    TEST_CHECK(*manager.springValue(id) == 1.0F);
    const float settleTime = static_cast<float>(frames) * kDt;
    TEST_CHECK(settleTime >= 0.3F);
    TEST_CHECK(settleTime <= 2.0F);
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{1.5F, 1000.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    float previous = 0.0F;
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
      const float x = *manager.springValue(id);
      TEST_CHECK(x >= previous - kTol);
      TEST_CHECK(x <= 1.0F + kTol);
      previous = x;
    }

    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(id) == 1.0F);
  }

  {
    AnimationManager clamped;
    AnimationManager reference;
    const auto clampedId =
        clamped.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    const auto referenceId =
        reference.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    clamped.tickSprings(0.2F);
    reference.tickSprings(0.032F);
    TEST_NEAR(*clamped.springValue(clampedId), *reference.springValue(referenceId), 1e-6F);

    for (int i = 0; i < 200; ++i) {
      clamped.tickSprings(0.032F);
    }
    const float x = *clamped.springValue(clampedId);
    TEST_CHECK(std::isfinite(x));
    TEST_CHECK(x >= 0.0F);
    TEST_CHECK(x <= 2.0F);
  }

  {
    AnimationManager manager;
    const auto a =
        manager.animateSpring(0.0F, 0.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    const auto b =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    manager.tickSprings(kDt);
    TEST_CHECK(manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(a) == 0.0F);
    TEST_CHECK(*manager.springValue(b) >= 0.0F);

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(b) == 1.0F);

    TEST_CHECK(!manager.springValue(0).has_value());
    TEST_CHECK(!manager.springValue(999).has_value());
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    for (int i = 0; i < 5; ++i) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(manager.hasActiveSprings());
    const float before = *manager.springValue(id);

    TEST_CHECK(manager.retargetSpring(id, 2.0F));
    TEST_CHECK(*manager.springValue(id) == before);

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(*manager.springValue(id) == 2.0F);

    TEST_CHECK(!manager.retargetSpring(0, 2.0F));
    TEST_CHECK(!manager.retargetSpring(999, 2.0F));
  }

  {
    AnimationManager manager;
    AnimationManager::Id bId = 0;
    bool created = false;

    const auto a = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{1.0F, 1600.0F},
        [&](float) {
          if (!created) {
            created = true;
            bId = manager.animateSpring(
                0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol
            );
          }
        },
        {}, nullptr, kTol, kTol
    );
    (void)a;

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }

    TEST_CHECK(created);
    TEST_CHECK(bId != 0);
    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(bId) == 1.0F);
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    for (int i = 0; i < 5; ++i) {
      manager.tickSprings(kDt);
    }
    float xBefore = *manager.springValue(id);
    manager.tickSprings(kDt);
    const float xTwoBefore = xBefore;
    xBefore = *manager.springValue(id);
    const float lastDelta = xBefore - xTwoBefore;

    TEST_CHECK(manager.retargetSpring(id, 2.0F));
    TEST_CHECK(std::abs(*manager.springValue(id) - xBefore) < 1e-4F);

    manager.tickSprings(kDt);
    const float firstDelta = *manager.springValue(id) - xBefore;
    const float scale = std::abs(lastDelta) > kDt ? std::abs(lastDelta) : kDt;
    TEST_CHECK(std::abs(firstDelta - lastDelta) < 0.5F * scale);

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(*manager.springValue(id) == 2.0F);
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(*manager.springValue(id) == 1.0F);

    TEST_CHECK(manager.retargetSpring(id, 0.0F));
    TEST_CHECK(manager.hasActiveSprings());

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(*manager.springValue(id) == 0.0F);
  }

  {
    AnimationManager retarget;
    AnimationManager control;
    const auto retargetId =
        retarget.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    const auto controlId =
        control.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    for (int i = 0; i < 4; ++i) {
      retarget.tickSprings(kDt);
      control.tickSprings(kDt);
    }
    const float previous = *retarget.springValue(retargetId);
    retarget.tickSprings(kDt);
    control.tickSprings(kDt);
    const float xBefore = *retarget.springValue(retargetId);
    const float lastDelta = xBefore - previous;

    TEST_CHECK(retarget.retargetSpringSpec(retargetId, SpringSpec{0.6F, 800.0F}));
    TEST_CHECK(std::abs(*retarget.springValue(retargetId) - xBefore) < 1e-4F);

    retarget.tickSprings(kDt);
    const float firstDelta = *retarget.springValue(retargetId) - xBefore;
    const float scale = std::abs(lastDelta) > kDt ? std::abs(lastDelta) : kDt;
    TEST_CHECK(std::abs(firstDelta - lastDelta) < 0.5F * scale);

    for (int i = 0; i < 10; ++i) {
      retarget.tickSprings(kDt);
      control.tickSprings(kDt);
    }
    TEST_CHECK(std::abs(*retarget.springValue(retargetId) - *control.springValue(controlId)) > 1e-3F);

    for (float t = 0.0F; t < 5.0F && retarget.hasActiveSprings(); t += kDt) {
      retarget.tickSprings(kDt);
    }
    TEST_CHECK(*retarget.springValue(retargetId) == 1.0F);
  }

  {
    AnimationManager manager;
    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(*manager.springValue(id) == 1.0F);

    TEST_CHECK(manager.retargetSpringSpec(id, SpringSpec{1.5F, 1000.0F}));
    TEST_CHECK(manager.hasActiveSprings());

    manager.tickSprings(kDt);
    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(*manager.springValue(id) == 1.0F);
  }

  {
    AnimationManager manager;
    TEST_CHECK(!manager.retargetSpringSpec(0, SpringSpec{1.0F, 1600.0F}));
    TEST_CHECK(!manager.retargetSpringSpec(999, SpringSpec{1.0F, 1600.0F}));
  }

  {
    AnimationManager manager;
    int completes = 0;
    const auto id = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, [&] { ++completes; }, nullptr, kTol, kTol
    );
    (void)id;
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 1);
  }

  {
    AnimationManager manager;
    int completes = 0;
    manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, [&] { ++completes; }, nullptr, kTol, kTol
    );
    for (int i = 0; i < 5; ++i) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 0);
  }

  {
    AnimationManager manager;
    int completes = 0;
    const auto id = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, [&] { ++completes; }, nullptr, kTol, kTol
    );
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 1);
    manager.retargetSpring(id, 0.0F);
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 2);
  }

  {
    AnimationManager manager;
    int completes = 0;
    const auto id = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, [&] { ++completes; }, nullptr, kTol, kTol
    );
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 1);
    manager.retargetSpring(id, 0.0F);
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 2);
    manager.retargetSpring(id, 1.0F);
    for (float t = 0.0F; t < 5.0F && manager.hasActiveSprings(); t += kDt) {
      manager.tickSprings(kDt);
    }
    TEST_CHECK(completes == 3);
  }

  {
    AnimationManager manager;
    int someOwner = 0;
    const auto a = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, &someOwner, kTol, kTol
    );
    const auto b = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, &someOwner, kTol, kTol
    );
    manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol
    );
    TEST_CHECK(manager.hasActiveSprings());
    manager.cancelForOwner(&someOwner);
    TEST_CHECK(manager.hasActiveSprings());
    TEST_CHECK(!manager.springValue(a).has_value());
    TEST_CHECK(!manager.springValue(b).has_value());
  }

  {
    AnimationManager manager;
    const auto id = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol
    );
    manager.cancelForOwner(nullptr);
    TEST_CHECK(manager.hasActiveSprings());
    TEST_CHECK(manager.springValue(id).has_value());
  }

  {
    AnimationManager manager;
    const auto id = manager.animateSpring(
        0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol
    );
    TEST_CHECK(manager.springValue(id).has_value());
    manager.cancel(id);
    TEST_CHECK(!manager.springValue(id).has_value());
    TEST_CHECK(!manager.hasActiveSprings());
  }

  {
    AnimationManager manager;
    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(manager.hasActiveSprings());
    manager.cancelAll();
    TEST_CHECK(!manager.hasActiveSprings());
  }

  return 0;
}
