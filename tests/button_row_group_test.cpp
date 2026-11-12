#include "render/core/renderer.h"
#include "render/core/texture_manager.h"
#include "tests/test_check.h"
#include "ui/controls/button.h"
#include "ui/style.h"

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
  const auto originalStyle = Style::cornerStyle();
  const auto originalDs = Style::designSystem();
  const auto originalShape = Style::cornerShape();
  Style::setCornerShape(Style::CornerShapePreset::Standard);

  StubRenderer renderer;
  const float md = Style::scaledRadiusMd(1.0F);
  const float sm = Style::scaledRadiusSm(1.0F);

  auto radiiFor = [&](Style::CornerStyle cs, FlexRowPosition pos) {
    Style::setCornerStyle(cs);
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    btn.setRowPosition(pos);
    return btn.currentRadiiForTest();
  };

  // Fixed ignores grouping: uniform md.
  {
    Radii r = radiiFor(Style::CornerStyle::Fixed, FlexRowPosition::First);
    TEST_NEAR(r.tl, md, 1e-4F);
    TEST_NEAR(r.tr, md, 1e-4F);
    TEST_NEAR(r.br, md, 1e-4F);
    TEST_NEAR(r.bl, md, 1e-4F);
  }

  // Material + Alone: uniform md.
  {
    Radii r = radiiFor(Style::CornerStyle::Material, FlexRowPosition::Alone);
    TEST_NEAR(r.tl, md, 1e-4F);
    TEST_NEAR(r.tr, md, 1e-4F);
    TEST_NEAR(r.br, md, 1e-4F);
    TEST_NEAR(r.bl, md, 1e-4F);
  }

  // Material + First: left md, right sm.
  {
    Radii r = radiiFor(Style::CornerStyle::Material, FlexRowPosition::First);
    TEST_NEAR(r.tl, md, 1e-4F);
    TEST_NEAR(r.bl, md, 1e-4F);
    TEST_NEAR(r.tr, sm, 1e-4F);
    TEST_NEAR(r.br, sm, 1e-4F);
  }

  // Material + Middle: all sm.
  {
    Radii r = radiiFor(Style::CornerStyle::Material, FlexRowPosition::Middle);
    TEST_NEAR(r.tl, sm, 1e-4F);
    TEST_NEAR(r.tr, sm, 1e-4F);
    TEST_NEAR(r.br, sm, 1e-4F);
    TEST_NEAR(r.bl, sm, 1e-4F);
  }

  // Material + Last: left sm, right md.
  {
    Radii r = radiiFor(Style::CornerStyle::Material, FlexRowPosition::Last);
    TEST_NEAR(r.tl, sm, 1e-4F);
    TEST_NEAR(r.bl, sm, 1e-4F);
    TEST_NEAR(r.tr, md, 1e-4F);
    TEST_NEAR(r.br, md, 1e-4F);
  }

  // Material First, Sharp shape → grow on press (square rounds out).
  {
    Style::setCornerStyle(Style::CornerStyle::Material);
    Style::setDesignSystem(Style::DesignSystem::Expressive);
    Style::setCornerShape(Style::CornerShapePreset::Sharp);
    Button b;
    b.setSize(60.0F, 60.0F);
    b.layout(renderer);
    b.setRowPosition(FlexRowPosition::First);
    auto rest = b.currentRadiiForTest();
    TEST_NEAR(rest.tl, 0.0F, 1e-4F);
    b.setPressedVisual(true);
    auto pressed = b.currentRadiiForTest();
    TEST_CHECK(pressed.tl > 0.001F);
    Style::setDesignSystem(originalDs);
    Style::setCornerShape(Style::CornerShapePreset::Standard);
  }

  // cornerStyle toggle mid-life re-applies radii.
  {
    Style::setCornerStyle(Style::CornerStyle::Material);
    Button btn;
    btn.setSize(60.0F, 60.0F);
    btn.layout(renderer);
    btn.setRowPosition(FlexRowPosition::First);
    TEST_NEAR(btn.currentRadiiForTest().tr, sm, 1e-4F);

    Style::setCornerStyle(Style::CornerStyle::Fixed);
    Radii fixed = btn.currentRadiiForTest();
    TEST_NEAR(fixed.tr, md, 1e-4F);
    TEST_NEAR(fixed.tl, md, 1e-4F);

    Style::setCornerStyle(Style::CornerStyle::Material);
    TEST_NEAR(btn.currentRadiiForTest().tr, sm, 1e-4F);
  }

  Style::setCornerStyle(originalStyle);
  Style::setCornerShape(originalShape);
  return 0;
}
