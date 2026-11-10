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

  // AT-DES-10: smooth preset forces effects.default (no overshoot)
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setMotionPreset(Style::MotionPreset::Smooth);
    MotionService& ms = MotionService::instance();
    SpringSpec spec = ms.resolve(SpringToken::SpatialDefault);
    TEST_NEAR(spec.dampingRatio, 1.0F, 1e-4F);
  }

  // Custom preset: baseline preserved
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setMotionPreset(Style::MotionPreset::Custom);
    MotionService& ms = MotionService::instance();
    SpringSpec spec = ms.resolve(SpringToken::SpatialDefault);
    SpringSpec expected = SpringTokenResolver().resolve(SpringToken::SpatialDefault);
    TEST_NEAR(spec.dampingRatio, expected.dampingRatio, 1e-4F);
  }

  // Classic design system: preset is inert
  {
    Style::setDesignSystem(Style::DesignSystem::Custom);
    Style::setMotionPreset(Style::MotionPreset::Playful);
    MotionService& ms = MotionService::instance();
    SpringSpec spec = ms.resolve(SpringToken::SpatialDefault);
    SpringSpec baseline = SpringTokenResolver().resolve(SpringToken::SpatialDefault);
    TEST_NEAR(spec.dampingRatio, baseline.dampingRatio, 1e-4F);
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
