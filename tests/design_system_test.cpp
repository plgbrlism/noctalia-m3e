#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto originalDs = Style::designSystem();
  const float originalScale = Style::cornerRadiusScale();

  // Test 1: designSystem accessor round-trip
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    TEST_CHECK(Style::designSystem() == Style::DesignSystem::Expressive);
    Style::setDesignSystem(Style::DesignSystem::Custom);
    TEST_CHECK(Style::designSystem() == Style::DesignSystem::Custom);
  }

  // Test 2: slider applies in both modes
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setCornerRadiusScale(1.0F);
    const float baseMd = Style::scaledRadiusMd(1.0F);

    Style::setCornerRadiusScale(2.0F);
    const float mdExpressive = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdExpressive, baseMd * 2.0F, 1e-4F);  // slider doubles in Expressive

    Style::setDesignSystem(Style::DesignSystem::Custom);
    Style::setCornerRadiusScale(2.0F);
    const float mdCustom2 = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdCustom2, baseMd * 2.0F, 1e-4F);   // slider doubles in Custom too
  }

  Style::setDesignSystem(originalDs);
  Style::setCornerRadiusScale(originalScale);
  return 0;
}
