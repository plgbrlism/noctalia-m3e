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
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(requests == 1);

    manager.animateSpring(0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(requests == 1);
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    manager.animateSpring(0.0F, 1.0F, SpringSpec{1.0F, 1600.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.6F, 800.0F}, [](float) {}, {}, nullptr, kTol, kTol);

    bool sawActiveRequest = false;
    int frames = 0;
    while (manager.hasActiveSprings() && frames < 400) {
      requests = 0;
      manager.tickSprings(kDt);
      if (requests >= 1) {
        sawActiveRequest = true;
      }
      TEST_CHECK(!manager.hasActive());
      ++frames;
    }

    TEST_CHECK(sawActiveRequest);
    TEST_CHECK(!manager.hasActiveSprings());
    TEST_CHECK(!manager.hasAnyActive());
    TEST_CHECK(frames < 400);

    for (int i = 0; i < 10; ++i) {
      requests = 0;
      manager.tickSprings(kDt);
      TEST_CHECK(requests == 0);
    }
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    const auto a =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(requests == 1);

    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(requests == 1);

    manager.retargetSpring(a, 2.0F);
    TEST_CHECK(requests == 1);

    manager.tickSprings(kDt);
    TEST_CHECK(requests == 2);
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    for (int i = 0; i < 1000 && manager.hasActiveSprings(); ++i) {
      manager.tickSprings(kDt);
    }
    for (int i = 0; i < 5; ++i) {
      requests = 0;
      manager.tickSprings(kDt);
      TEST_CHECK(requests == 0);
    }

    requests = 0;
    manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    TEST_CHECK(requests == 1);

    manager.tickSprings(kDt);
    TEST_CHECK(requests == 2);
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.8F, 380.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    for (int i = 0; i < 1000 && manager.hasActiveSprings(); ++i) {
      manager.tickSprings(kDt);
    }
    requests = 0;
    manager.tickSprings(kDt);
    TEST_CHECK(requests == 0);

    requests = 0;
    TEST_CHECK(manager.retargetSpring(id, 0.0F));
    TEST_CHECK(requests == 1);

    manager.tickSprings(kDt);
    TEST_CHECK(requests == 2);
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    const auto id =
        manager.animateSpring(0.0F, 1.0F, SpringSpec{0.6F, 800.0F}, [](float) {}, {}, nullptr, kTol, kTol);
    manager.tickSprings(kDt);
    requests = 0;

    TEST_CHECK(manager.retargetSpring(id, 2.0F));
    TEST_CHECK(requests == 0);

    manager.tickSprings(kDt);
    TEST_CHECK(requests == 1);
  }

  {
    AnimationManager manager;
    int requests = 0;
    manager.setFrameRequest([&] { ++requests; });

    manager.animate(0.0F, 1.0F, 100.0F, Easing::Linear, [](float) {});
    TEST_CHECK(requests == 0);
    TEST_CHECK(manager.hasActive());
    TEST_CHECK(manager.hasAnyActive());

    requests = 0;
    manager.tickSprings(kDt);
    TEST_CHECK(requests == 0);
    TEST_CHECK(manager.hasActive());
  }

  return 0;
}
