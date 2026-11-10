#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto originalDs = Style::designSystem();
  const float originalScale = Style::cornerRadiusScale();

  // AT-DES-01: Expressive — slider ignored
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setCornerRadiusScale(2.0F);
    const float mdExpressive = Style::scaledRadiusMd(1.0F);
    Style::setCornerRadiusScale(1.0F);
    const float baseMd = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdExpressive, baseMd, 1e-4F);  // slider ignored
  }

  // AT-DES-02: Custom — slider respected
  {
    Style::setDesignSystem(Style::DesignSystem::Custom);
    Style::setCornerRadiusScale(2.0F);
    const float mdCustom2 = Style::scaledRadiusMd(1.0F);
    Style::setCornerRadiusScale(1.0F);
    const float baseMd = Style::scaledRadiusMd(1.0F);
    TEST_NEAR(mdCustom2, baseMd * 2.0F, 1e-4F);
  }

  // AT-DES-04: mode switch reflects in next call
  {
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setCornerRadiusScale(2.0F);
    const float expressive = Style::scaledRadiusMd(1.0F);
    Style::setDesignSystem(Style::DesignSystem::Custom);
    const float custom = Style::scaledRadiusMd(1.0F);
    TEST_CHECK(custom > expressive);  // custom+slider2 > expressive
  }

  Style::setDesignSystem(originalDs);
  Style::setCornerRadiusScale(originalScale);
  return 0;
}
