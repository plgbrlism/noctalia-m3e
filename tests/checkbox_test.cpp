#include "render/core/renderer.h"
#include "render/core/texture_manager.h"
#include "render/scene/input_area.h"
#include "tests/test_check.h"
#include "ui/controls/box.h"
#include "ui/controls/checkbox.h"
#include "ui/controls/glyph.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <linux/input-event-codes.h>

#include <cmath>
#include <cstdlib>
#include <string_view>
#include <vector>

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

    TextMetrics measureGlyph(char32_t, float) override {
      return TextMetrics{
          .width = 18.0F,
          .left = 1.5F,
          .right = 19.5F,
          .top = -15.0F,
          .bottom = 3.0F,
          .inkTop = -15.0F,
          .inkBottom = 3.0F,
          .inkLeft = 1.5F,
          .inkRight = 19.5F,
      };
    }

    TextureManager& textureManager() override { std::abort(); }
    float renderScale() const noexcept override { return 1.0F; }
  };

  template <typename T> T* childAt(Checkbox& checkbox, std::size_t index) {
    auto* child = dynamic_cast<T*>(checkbox.children()[index].get());
    TEST_CHECK(child != nullptr);
    return child;
  }

  Box* stateLayer(Checkbox& checkbox) { return childAt<Box>(checkbox, 0); }
  Box* container(Checkbox& checkbox) { return childAt<Box>(checkbox, 1); }
  Glyph* glyph(Checkbox& checkbox) { return childAt<Glyph>(checkbox, 2); }
  InputArea* inputArea(Checkbox& checkbox) { return childAt<InputArea>(checkbox, 3); }

} // namespace

int main() {
  StubRenderer renderer;

  // AT-CB-01: 48/40/18/r2 geometry at 1.0x.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    TEST_NEAR(checkbox.width(), 48.0F, 1e-4F);
    TEST_NEAR(checkbox.height(), 48.0F, 1e-4F);

    auto* layer = stateLayer(checkbox);
    TEST_NEAR(layer->x(), 4.0F, 1e-4F);
    TEST_NEAR(layer->y(), 4.0F, 1e-4F);
    TEST_NEAR(layer->width(), 40.0F, 1e-4F);
    TEST_NEAR(layer->height(), 40.0F, 1e-4F);
    TEST_NEAR(layer->style().radius.tl, 20.0F, 1e-4F);

    auto* box = container(checkbox);
    TEST_NEAR(box->x(), 15.0F, 1e-4F);
    TEST_NEAR(box->y(), 15.0F, 1e-4F);
    TEST_NEAR(box->width(), 18.0F, 1e-4F);
    TEST_NEAR(box->height(), 18.0F, 1e-4F);
    TEST_NEAR(box->style().radius.tl, 2.0F, 1e-4F);

    auto* icon = glyph(checkbox);
    TEST_NEAR(icon->width(), 18.0F, 1e-4F);
    TEST_NEAR(icon->height(), 18.0F, 1e-4F);

    auto* area = inputArea(checkbox);
    TEST_NEAR(area->width(), 48.0F, 1e-4F);
    TEST_NEAR(area->height(), 48.0F, 1e-4F);
  }

  // AT-CB-02: 1.25x and 1.5x scaling.
  {
    Checkbox checkbox;
    checkbox.setScale(1.25F);
    checkbox.layout(renderer);

    TEST_NEAR(checkbox.width(), 60.0F, 1e-4F);
    auto* layer = stateLayer(checkbox);
    TEST_NEAR(layer->x(), 5.0F, 1e-4F);
    TEST_NEAR(layer->width(), 50.0F, 1e-4F);
    auto* box = container(checkbox);
    TEST_NEAR(box->x(), 18.75F, 1e-4F);
    TEST_NEAR(box->width(), 22.5F, 1e-4F);
    TEST_NEAR(box->style().radius.tl, 2.5F, 1e-4F);

    checkbox.setScale(1.5F);
    checkbox.layout(renderer);

    TEST_NEAR(checkbox.width(), 72.0F, 1e-4F);
    layer = stateLayer(checkbox);
    TEST_NEAR(layer->x(), 6.0F, 1e-4F);
    TEST_NEAR(layer->width(), 60.0F, 1e-4F);
    box = container(checkbox);
    TEST_NEAR(box->x(), 22.5F, 1e-4F);
    TEST_NEAR(box->width(), 27.0F, 1e-4F);
    TEST_NEAR(box->style().radius.tl, 3.0F, 1e-4F);
  }

  // AT-CB-03: Hover/press alpha targets; focus adds no ring or alpha.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    auto* layer = stateLayer(checkbox);
    auto* area = inputArea(checkbox);

    area->dispatchEnter(24.0F, 24.0F);
    TEST_NEAR(layer->style().fill.a, Style::checkboxStateLayerHoverAlpha, 1e-4F);

    area->dispatchFocusGain();
    TEST_NEAR(layer->style().fill.a, Style::checkboxStateLayerHoverAlpha, 1e-4F);
    TEST_NEAR(layer->style().borderWidth, 0.0F, 1e-4F);

    area->dispatchPress(24.0F, 24.0F, BTN_LEFT, true);
    TEST_NEAR(layer->style().fill.a, Style::checkboxStateLayerPressedAlpha, 1e-4F);
    area->dispatchPress(24.0F, 24.0F, BTN_LEFT, false);

    area->dispatchFocusLoss();
    TEST_NEAR(layer->style().fill.a, Style::checkboxStateLayerHoverAlpha, 1e-4F);
    TEST_NEAR(layer->style().borderWidth, 0.0F, 1e-4F);

    area->dispatchLeave();
    TEST_NEAR(layer->style().fill.a, 0.0F, 1e-4F);
  }

  // AT-CB-04: Disabled state.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    auto* layer = stateLayer(checkbox);
    auto* area = inputArea(checkbox);

    checkbox.setEnabled(false);
    TEST_NEAR(checkbox.opacity(), 0.55F, 1e-4F);
    TEST_NEAR(layer->style().fill.a, 0.0F, 1e-4F);
    TEST_CHECK(!area->enabled());

    area->dispatchPress(24.0F, 24.0F, BTN_LEFT, true);
    area->dispatchPress(24.0F, 24.0F, BTN_LEFT, false);
    TEST_CHECK(!checkbox.checked());
  }

  // AT-CB-05: Checked/error/indeterminate glyph and colors.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    auto* box = container(checkbox);
    auto* icon = glyph(checkbox);

    TEST_CHECK(!icon->visible());
    TEST_CHECK(box->style().fill == clearColor());
    TEST_CHECK(box->style().border == colorForRole(ColorRole::OnSurfaceVariant));

    checkbox.setChecked(true);
    TEST_CHECK(checkbox.checked());
    TEST_CHECK(!checkbox.indeterminate());
    TEST_CHECK(icon->visible());
    TEST_CHECK(box->style().fill == colorForRole(ColorRole::Primary));

    checkbox.setError(true);
    TEST_CHECK(checkbox.error());
    TEST_CHECK(box->style().fill == colorForRole(ColorRole::Error));
    TEST_CHECK(box->style().borderWidth == 0.0F);

    checkbox.setChecked(false);
    checkbox.setError(false);
    checkbox.setIndeterminate(true);
    TEST_CHECK(checkbox.indeterminate());
    TEST_CHECK(icon->visible());
    TEST_CHECK(box->style().fill == colorForRole(ColorRole::Primary));

    checkbox.setIndeterminate(false);
    checkbox.setError(true);
    TEST_CHECK(box->style().border == colorForRole(ColorRole::Error));
    TEST_NEAR(box->style().borderWidth, Style::checkboxOutlineWidth, 1e-4F);
  }

  // AT-CB-06: Click target bounds.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    auto* area = inputArea(checkbox);
    bool changed = false;
    checkbox.setOnChange([&changed](bool) { changed = true; });

    area->dispatchPress(47.0F, 47.0F, BTN_LEFT, true);
    area->dispatchPress(47.0F, 47.0F, BTN_LEFT, false);
    TEST_CHECK(checkbox.checked());
    TEST_CHECK(changed);

    changed = false;
    area->dispatchPress(60.0F, 60.0F, BTN_LEFT, true);
    area->dispatchPress(60.0F, 60.0F, BTN_LEFT, false);
    TEST_CHECK(checkbox.checked());
    TEST_CHECK(!changed);
  }

  // AT-CB-07: No layout mutation during state changes.
  {
    Checkbox checkbox;
    checkbox.setScale(1.0F);
    checkbox.layout(renderer);

    auto* layer = stateLayer(checkbox);
    auto* box = container(checkbox);
    auto* icon = glyph(checkbox);
    auto* area = inputArea(checkbox);

    const float width = checkbox.width();
    const float height = checkbox.height();
    const float layerX = layer->x();
    const float layerY = layer->y();
    const float layerWidth = layer->width();
    const float layerHeight = layer->height();
    const float boxX = box->x();
    const float boxY = box->y();
    const float boxWidth = box->width();
    const float boxHeight = box->height();
    const float iconX = icon->x();
    const float iconY = icon->y();
    const float areaWidth = area->width();
    const float areaHeight = area->height();

    checkbox.setChecked(true);
    checkbox.setIndeterminate(true);
    checkbox.setError(true);
    checkbox.setEnabled(false);

    TEST_NEAR(checkbox.width(), width, 1e-4F);
    TEST_NEAR(checkbox.height(), height, 1e-4F);
    TEST_NEAR(layer->x(), layerX, 1e-4F);
    TEST_NEAR(layer->y(), layerY, 1e-4F);
    TEST_NEAR(layer->width(), layerWidth, 1e-4F);
    TEST_NEAR(layer->height(), layerHeight, 1e-4F);
    TEST_NEAR(box->x(), boxX, 1e-4F);
    TEST_NEAR(box->y(), boxY, 1e-4F);
    TEST_NEAR(box->width(), boxWidth, 1e-4F);
    TEST_NEAR(box->height(), boxHeight, 1e-4F);
    TEST_NEAR(icon->x(), iconX, 1e-4F);
    TEST_NEAR(icon->y(), iconY, 1e-4F);
    TEST_NEAR(area->width(), areaWidth, 1e-4F);
    TEST_NEAR(area->height(), areaHeight, 1e-4F);
  }

  return 0;
}
