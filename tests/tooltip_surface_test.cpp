#include "tests/test_check.h"
#include "ui/palette.h"

int main() {
  // AT-UI-02: border off → elevated surface
  {
    const auto r = resolveSurfaceRoles(false);
    TEST_CHECK(r.background == ColorRole::SurfaceContainerHighest);
    TEST_CHECK(r.text == ColorRole::OnSurface);
  }

  // AT-UI-03: border on → current surface (regression)
  {
    const auto r = resolveSurfaceRoles(true);
    TEST_CHECK(r.background == ColorRole::Surface);
    TEST_CHECK(r.text == ColorRole::OnSurface);
  }

  return 0;
}
