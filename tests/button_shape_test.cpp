#include "render/core/renderer.h"
#include "render/core/texture_manager.h"
#include "tests/test_check.h"
#include "ui/controls/button.h"
#include "ui/style.h"

#include <cmath>
#include <cstdlib>

namespace {
  class StubRenderer final : public Renderer {
  public:
    TextMetrics measureText(
        std::string_view, float, const TextFont&, float, int, TextAlign, TextEllipsize, bool
    ) override {
      return TextMetrics{};
    }

    TextMetrics measureFont(float fontSize, const TextFont&) override { return TextMetrics{.bottom = fontSize}; }

    void measureTextCursorStops(
        std::string_view, float, const std::vector<std::size_t>&, std::vector<float>&, const TextFont&
    ) override {}

    void measureTextCursorStopsWrapped(
        std::string_view, float, const std::vector<std::size_t>&, float, std::vector<TextCursorStop>&, const TextFont&
    ) override {}

    TextMetrics measureGlyph(char32_t, float) override { return TextMetrics{}; }

    TextureManager& textureManager() override { std::abort(); }
    float renderScale() const noexcept override { return 1.0F; }
  };
} // namespace

int main() {
  StubRenderer renderer;

  // 1. Rest radii default (4.0) at scale 1.0
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, 4.0F, 1e-4F);
    TEST_NEAR(r.tr, 4.0F, 1e-4F);
    TEST_NEAR(r.br, 4.0F, 1e-4F);
    TEST_NEAR(r.bl, 4.0F, 1e-4F);
  }

  // 2. Pressed radii (12.0) at scale 1.0
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    btn.setPressedVisual(true);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, 12.0F, 1e-4F);
    TEST_NEAR(r.tr, 12.0F, 1e-4F);
    TEST_NEAR(r.br, 12.0F, 1e-4F);
    TEST_NEAR(r.bl, 12.0F, 1e-4F);
  }

  // 3. Clamp (AT-SHP-01): button size 8.0x8.0 forces max radius to 4.0
  {
    Button btn;
    btn.setSize(8.0F, 8.0F);
    btn.layout(renderer);
    btn.setPressedVisual(true);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, 4.0F, 1e-4F);
    TEST_NEAR(r.tr, 4.0F, 1e-4F);
    TEST_NEAR(r.br, 4.0F, 1e-4F);
    TEST_NEAR(r.bl, 4.0F, 1e-4F);
  }

  // 4. Fractional scale (AT-SHP-02): 1.25x and 1.5x
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);

    btn.setContentScaleForTest(1.25F);
    Radii r125 = btn.currentRadiiForTest();
    TEST_NEAR(r125.tl, 4.0F * 1.25F, 1e-4F);

    btn.setPressedVisual(true);
    Radii p125 = btn.currentRadiiForTest();
    TEST_NEAR(p125.tl, 12.0F * 1.25F, 1e-4F);

    btn.setContentScaleForTest(1.5F);
    Radii p150 = btn.currentRadiiForTest();
    TEST_NEAR(p150.tl, 12.0F * 1.5F, 1e-4F);
  }

  return 0;
}
