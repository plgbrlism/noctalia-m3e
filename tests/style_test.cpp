#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto originalShape = Style::cornerShape();
  TEST_CHECK(Style::cornerShape() == Style::CornerShapePreset::Standard);

  Style::setCornerShape(Style::CornerShapePreset::Sharp);
  TEST_NEAR(Style::scaledRadiusMd(1.0F), 0.0F, 1e-4F);
  TEST_NEAR(Style::scaledRadiusSm(1.0F), 0.0F, 1e-4F);

  Style::setCornerShape(Style::CornerShapePreset::Standard);
  TEST_NEAR(Style::scaledRadiusMd(1.0F), 6.0F, 1e-4F);
  TEST_NEAR(Style::scaledRadiusSm(1.0F), 3.0F, 1e-4F);

  Style::setCornerShape(Style::CornerShapePreset::Round);
  TEST_NEAR(Style::scaledRadiusMd(1.0F), 12.0F, 1e-4F);
  TEST_NEAR(Style::scaledRadiusSm(1.0F), 6.0F, 1e-4F);

  int emissions = 0;
  auto connection = Style::cornerShapeChanged().connect([&emissions] { ++emissions; });
  Style::setCornerShape(Style::CornerShapePreset::Standard);
  Style::setCornerShape(Style::CornerShapePreset::Standard);
  TEST_CHECK(emissions == 1);
  connection.disconnect();

  Style::setCornerShape(Style::CornerShapePreset::Sharp);
  TEST_NEAR(Style::scaledRadiusMd(1.5F), 0.0F, 1e-4F);

  Style::setCornerShape(Style::CornerShapePreset::Standard);
  TEST_NEAR(Style::scaledRadiusMd(1.5F), 9.0F, 1e-4F);
  TEST_NEAR(Style::scaledRadiusSm(1.5F), 4.5F, 1e-4F);

  Style::setCornerShape(Style::CornerShapePreset::Round);
  TEST_NEAR(Style::scaledRadiusMd(1.5F), 18.0F, 1e-4F);
  TEST_NEAR(Style::scaledRadiusSm(1.5F), 9.0F, 1e-4F);

  Style::setCornerShape(originalShape);
  return 0;
}
