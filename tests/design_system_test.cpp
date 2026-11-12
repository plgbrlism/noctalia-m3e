#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto originalDs = Style::designSystem();
  const auto originalShape = Style::cornerShape();

  // Test 1: designSystem accessor round-trip
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    TEST_CHECK(Style::designSystem() == Style::DesignSystem::Expressive);
    Style::setDesignSystem(Style::DesignSystem::Custom);
    TEST_CHECK(Style::designSystem() == Style::DesignSystem::Custom);
  }

  // Test 2: corner shape applies in both modes
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setCornerShape(Style::CornerShapePreset::Standard);
    const float baseMd = Style::scaledRadiusMd(1.0F);

    Style::setCornerShape(Style::CornerShapePreset::Round);
    const float mdExpressive = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdExpressive, baseMd * 2.0F, 1e-4F);  // round doubles in Expressive

    Style::setDesignSystem(Style::DesignSystem::Custom);
    Style::setCornerShape(Style::CornerShapePreset::Round);
    const float mdCustom2 = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdCustom2, baseMd * 2.0F, 1e-4F);   // round doubles in Custom too
  }

  Style::setDesignSystem(originalDs);
  Style::setCornerShape(originalShape);
  return 0;
}
