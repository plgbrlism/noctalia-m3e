#include "shell/panel/panel_reveal_helpers.h"
#include "tests/test_check.h"

int main() {
  using namespace vespertine::shell::panel::reveal;

  // AT-PANEL-01: Standard identity
  for (float p : {0.0F, 0.5F, 1.0F}) {
    auto t = chromeTransform(p, PanelMotion::Standard, RevealDirection::Down, true);
    TEST_NEAR(t.scaleX, 1.0F, 1e-4F);
    TEST_NEAR(t.scaleY, 1.0F, 1e-4F);
    TEST_NEAR(t.offsetX, 0.0F, 1e-4F);
    TEST_NEAR(t.offsetY, 0.0F, 1e-4F);
  }

  // AT-PANEL-02: Depth zoom (attached and detached same)
  for (bool attached : {true, false}) {
    auto t0 = chromeTransform(0.0F, PanelMotion::Depth, RevealDirection::Down, attached);
    TEST_NEAR(t0.scaleX, 0.92F, 1e-4F);
    TEST_NEAR(t0.scaleY, 0.92F, 1e-4F);
    auto t1 = chromeTransform(1.0F, PanelMotion::Depth, RevealDirection::Down, attached);
    TEST_NEAR(t1.scaleX, 1.00F, 1e-4F);
    TEST_NEAR(t1.scaleY, 1.00F, 1e-4F);
    TEST_NEAR(t1.offsetX, 0.0F, 1e-4F);
    TEST_NEAR(t1.offsetY, 0.0F, 1e-4F);
  }

  // AT-PANEL-05: Fluid — detached slides + squashes
  {
    auto down = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Down, false);
    TEST_CHECK(down.offsetY < 0.0F);
    TEST_CHECK(down.scaleX > 1.0F);
    TEST_NEAR(down.scaleY, 1.0F, 1e-4F);
    TEST_NEAR(down.offsetX, 0.0F, 1e-4F);

    auto up = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Up, false);
    TEST_CHECK(up.offsetY > 0.0F);
    TEST_CHECK(up.scaleX > 1.0F);

    auto right = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Right, false);
    TEST_CHECK(right.offsetX < 0.0F);
    TEST_CHECK(right.scaleY > 1.0F);

    auto left = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Left, false);
    TEST_CHECK(left.offsetX > 0.0F);
    TEST_CHECK(left.scaleY > 1.0F);

    // Identity at p=1
    auto end = chromeTransform(1.0F, PanelMotion::Fluid, RevealDirection::Down, false);
    TEST_NEAR(end.scaleX, 1.0F, 1e-4F);
    TEST_NEAR(end.scaleY, 1.0F, 1e-4F);
    TEST_NEAR(end.offsetX, 0.0F, 1e-4F);
    TEST_NEAR(end.offsetY, 0.0F, 1e-4F);
  }

  // AT-PANEL-05: Fluid — attached squash only, no offset
  {
    auto down = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Down, true);
    TEST_NEAR(down.offsetX, 0.0F, 1e-4F);
    TEST_NEAR(down.offsetY, 0.0F, 1e-4F);
    TEST_CHECK(down.scaleX > 1.0F);
  }

  // Clamp out-of-range
  {
    auto low = chromeTransform(-0.5F, PanelMotion::Fluid, RevealDirection::Down, false);
    auto high = chromeTransform(1.5F, PanelMotion::Fluid, RevealDirection::Down, false);
    TEST_NEAR(low.offsetY, -kFluidTravel, 1e-4F);   // clamped to p=0
    TEST_NEAR(high.offsetY, 0.0F, 1e-4F);           // clamped to p=1
  }

  // Blur region follows the same transform as the panel chrome.
  {
    const RevealRect body{20.0F, 30.0F, 200.0F, 100.0F};
    const float originX = body.x + body.width / 2.0F;
    const float originY = body.y + body.height / 2.0F;

    const auto identity = transformBodyRect(body, originX, originY, RevealTransform{});
    TEST_NEAR(identity.x, body.x, 1e-4F);
    TEST_NEAR(identity.y, body.y, 1e-4F);
    TEST_NEAR(identity.width, body.width, 1e-4F);
    TEST_NEAR(identity.height, body.height, 1e-4F);

    const auto depth = transformBodyRect(
        body, originX, originY, chromeTransform(0.0F, PanelMotion::Depth, RevealDirection::Down, false)
    );
    TEST_NEAR(depth.width, 0.92F * body.width, 1e-4F);
    TEST_NEAR(depth.height, 0.92F * body.height, 1e-4F);
    TEST_NEAR(depth.x + depth.width / 2.0F, originX, 1e-4F);
    TEST_NEAR(depth.y + depth.height / 2.0F, originY, 1e-4F);

    const auto fluidTransform = chromeTransform(0.0F, PanelMotion::Fluid, RevealDirection::Down, false);
    const auto fluid = transformBodyRect(body, originX, originY, fluidTransform);
    TEST_NEAR(fluid.width, fluidTransform.scaleX * body.width, 1e-4F);
    TEST_NEAR(fluid.y, body.y + fluidTransform.offsetY, 1e-4F);
    TEST_NEAR(fluid.x + fluid.width / 2.0F, originX, 1e-4F);
  }

  return 0;
}
