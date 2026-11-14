#include "tests/test_check.h"
#include "ui/style.h"

int main() {
  const auto origDs = Style::designSystem();
  const auto origCorner = Style::cornerStyle();

  // AT-DES-08: corner style gate (unit test on Style accessor only)
  {
    Style::setCornerStyle(Style::CornerStyle::Fixed);
    TEST_CHECK(Style::cornerStyle() == Style::CornerStyle::Fixed);
    Style::setCornerStyle(Style::CornerStyle::Material);
    TEST_CHECK(Style::cornerStyle() == Style::CornerStyle::Material);
  }

  Style::setDesignSystem(origDs);
  Style::setCornerStyle(origCorner);
  return 0;
}
