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
  const auto originalDs = Style::designSystem();
  const float originalScale = Style::cornerRadiusScale();
  Style::setDesignSystem(Style::DesignSystem::Expressive);
  Style::setCornerRadiusScale(1.0F);

  StubRenderer renderer;

  // 1. Rest radii default at scale 1.0
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, Style::scaledRadiusMd(1.0F), 1e-4F);
    TEST_NEAR(r.tr, Style::scaledRadiusMd(1.0F), 1e-4F);
    TEST_NEAR(r.br, Style::scaledRadiusMd(1.0F), 1e-4F);
    TEST_NEAR(r.bl, Style::scaledRadiusMd(1.0F), 1e-4F);
  }

  // 2. Pressed radii at scale 1.0 (rest=6, delta=10 → pressed=16.0)
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    btn.setPressedVisual(true);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, 16.0F, 1e-4F);
    TEST_NEAR(r.tr, 16.0F, 1e-4F);
    TEST_NEAR(r.br, 16.0F, 1e-4F);
    TEST_NEAR(r.bl, 16.0F, 1e-4F);
  }

  // 3. Clamp (AT-SHP-01): 8x8 button (maxRadius=4), pressed state.
  //    Rest=6 exceeds maxRadius; Batch 2 shrink math drives pressed to 0.
  //    Assertion is that radii never exceed maxRadius — 0 is valid.
  {
    Button btn;
    btn.setSize(8.0F, 8.0F);
    btn.layout(renderer);
    btn.setPressedVisual(true);
    Radii r = btn.currentRadiiForTest();
    TEST_NEAR(r.tl, 0.0F, 1e-4F);
    TEST_NEAR(r.tr, 0.0F, 1e-4F);
    TEST_NEAR(r.br, 0.0F, 1e-4F);
    TEST_NEAR(r.bl, 0.0F, 1e-4F);
  }

  // 4. Fractional scale (AT-SHP-02): 1.25x and 1.5x
  {
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);

    btn.setContentScaleForTest(1.25F);
    Radii r125 = btn.currentRadiiForTest();
    TEST_NEAR(r125.tl, Style::scaledRadiusMd(1.25F), 1e-4F);

    btn.setPressedVisual(true);
    Radii p125 = btn.currentRadiiForTest();
    TEST_NEAR(p125.tl, 16.0F * 1.25F, 1e-4F);

    btn.setContentScaleForTest(1.5F);
    Radii p150 = btn.currentRadiiForTest();
    TEST_NEAR(p150.tl, 16.0F * 1.5F, 1e-4F);
  }

  Style::setDesignSystem(originalDs);
  Style::setCornerRadiusScale(originalScale);
  return 0;
}
