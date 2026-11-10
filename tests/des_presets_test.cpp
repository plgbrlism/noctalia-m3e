#include "render/animation/motion_service.h"
#include "render/animation/motion_scheme.h"
#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto origDs = Style::designSystem();
  const auto origPreset = Style::motionPreset();
  const auto origCorner = Style::cornerStyle();

  // AT-DES-09: playful preset overrides token family
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setMotionPreset(Style::MotionPreset::Playful);
    MotionService& ms = MotionService::instance();
    SpringSpec requestedSpatial = ms.resolve(SpringToken::SpatialDefault);
    SpringSpec expectedFast = SpringTokenResolver().resolve(SpringToken::SpatialFast);
    TEST_NEAR(requestedSpatial.dampingRatio, expectedFast.dampingRatio, 1e-4F);
    TEST_NEAR(requestedSpatial.stiffness, expectedFast.stiffness, 1e-4F);
  }

  // AT-DES-10: smooth preset → Standard scheme spatial.default (0.9/700)
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setMotionPreset(Style::MotionPreset::Smooth);
    MotionService& ms = MotionService::instance();
    SpringSpec spec = ms.resolve(SpringToken::SpatialDefault);
    TEST_NEAR(spec.dampingRatio, 0.9F, 1e-4F);
    TEST_NEAR(spec.stiffness, 700.0F, 1e-4F);
  }

  // AT-DES-08: corner style gate (unit test on Style accessor only)
  {
    Style::setCornerStyle(Style::CornerStyle::Fixed);
    TEST_CHECK(Style::cornerStyle() == Style::CornerStyle::Fixed);
    Style::setCornerStyle(Style::CornerStyle::Material);
    TEST_CHECK(Style::cornerStyle() == Style::CornerStyle::Material);
  }

  Style::setDesignSystem(origDs);
  Style::setMotionPreset(origPreset);
  Style::setCornerStyle(origCorner);
  return 0;
}
