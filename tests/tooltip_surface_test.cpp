#include "shell/tooltip/tooltip_manager.h"
#include "tests/test_check.h"

int main() {
  // AT-UI-02: border off → elevated surface
  {
    const auto r = resolveTooltipRoles(false);
    TEST_CHECK(r.background == ColorRole::SurfaceContainerHighest);
    TEST_CHECK(r.text == ColorRole::OnSurface);
  }

  // AT-UI-03: border on → current surface (regression)
  {
    const auto r = resolveTooltipRoles(true);
    TEST_CHECK(r.background == ColorRole::Surface);
    TEST_CHECK(r.text == ColorRole::OnSurface);
  }

  return 0;
}
