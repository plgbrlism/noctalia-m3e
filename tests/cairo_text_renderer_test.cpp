#include "render/text/cairo_text_renderer.h"

#include <array>
#include <cmath>
#include <print>
#include <string>
#include <string_view>

namespace {

  bool sameMetrics(const CairoTextRenderer::TextMetrics& lhs, const CairoTextRenderer::TextMetrics& rhs) {
    constexpr float epsilon = 0.0001F;
    return std::abs(lhs.width - rhs.width) < epsilon
        && std::abs(lhs.top - rhs.top) < epsilon
        && std::abs(lhs.bottom - rhs.bottom) < epsilon
        && lhs.lineCount == rhs.lineCount;
  }

  // Wrapping-boundary cache correctness: a cached measurement at one width must
  // not reuse metrics computed for a different width.
  int wrappingBoundaryCase(CairoTextRenderer& renderer) {
    constexpr std::string_view text =
        "Error: Error while parsing table header: expected a comment or whitespace, saw 't'";
    constexpr float fontSize = 14.0F;
    const float naturalWidth = renderer.measure(1.0F, text, fontSize).width;
    constexpr std::array deltas{0.0005F, 0.001F, 0.002F, 0.003F, 0.004F, 0.006F};

    for (const float delta : deltas) {
      renderer.notifyFontConfigChanged();
      const auto above = renderer.measure(1.0F, text, fontSize, {FontWeight::Normal}, naturalWidth + delta, 3);
      renderer.notifyFontConfigChanged();
      const auto expectedBelow = renderer.measure(1.0F, text, fontSize, {FontWeight::Normal}, naturalWidth - delta, 3);
      if (above.lineCount == expectedBelow.lineCount) {
        continue;
      }

      renderer.notifyFontConfigChanged();
      [[maybe_unused]] const auto cachedAbove =
          renderer.measure(1.0F, text, fontSize, {FontWeight::Normal}, naturalWidth + delta, 3);
      const auto cachedBelow = renderer.measure(1.0F, text, fontSize, {FontWeight::Normal}, naturalWidth - delta, 3);
      if (cachedBelow.lineCount != expectedBelow.lineCount) {
        std::println(
            stderr,
            "cairo_text_renderer_test: FAIL: cached measurement at width {} reused metrics from width {} "
            "(expected {} lines, got {})",
            naturalWidth - delta, naturalWidth + delta, expectedBelow.lineCount, cachedBelow.lineCount
        );
        return 1;
      }
      return 0;
    }

    std::println(stderr, "cairo_text_renderer_test: FAIL: could not establish a wrapping boundary");
    return 1;
  }

  // The metrics cache is keyed by content scale: interleaving 1.0x and 1.25x
  // measurements of the same text must return per-scale-repeatable results, and
  // an intervening call at the other scale must not corrupt the cached result.
  int scaleKeyedCacheCase(CairoTextRenderer& renderer) {
    constexpr std::string_view text = "The quick brown fox";
    constexpr float fontSize = 16.0F;

    const auto oneA = renderer.measure(1.0F, text, fontSize);
    const auto hiA = renderer.measure(1.25F, text, fontSize);
    const auto oneB = renderer.measure(1.0F, text, fontSize); // cache hit at 1.0x
    const auto hiB = renderer.measure(1.25F, text, fontSize); // cache hit at 1.25x

    if (!sameMetrics(oneA, oneB)) {
      std::println(
          stderr,
          "cairo_text_renderer_test: FAIL: 1.0x measurement not repeatable across a 1.25x call "
          "(width {} vs {}, lines {} vs {})",
          oneA.width, oneB.width, oneA.lineCount, oneB.lineCount
      );
      return 1;
    }
    if (!sameMetrics(hiA, hiB)) {
      std::println(
          stderr,
          "cairo_text_renderer_test: FAIL: 1.25x measurement not repeatable across a 1.0x call "
          "(width {} vs {}, lines {} vs {})",
          hiA.width, hiB.width, hiA.lineCount, hiB.lineCount
      );
      return 1;
    }
    return 0;
  }

  int middleEllipsisSpacingCase(CairoTextRenderer& renderer) {
    const TextFont font{.family = "monospace"};
    for (const float scale : {1.0F, 1.25F, 1.5F, 2.0F}) {
      const auto retained = renderer.measure(scale, "MMM…MMM", 14.0F, font);
      const auto glyph = renderer.measure(scale, "M", 14.0F, font);
      const float budget = retained.width + glyph.width * 0.5F;
      const auto text = std::string(40, 'M');
      const auto compact =
          renderer.measure(scale, text, 14.0F, font, budget, 1, TextAlign::Start, TextEllipsize::Middle);
      if (std::abs(compact.width - retained.width) >= 0.01F || compact.lineCount != 1) {
        std::println(stderr, "cairo_text_renderer_test: FAIL: middle ellipsis spacing at scale {}", scale);
        return 1;
      }
      const auto again =
          renderer.measure(scale, text, 14.0F, font, budget, 1, TextAlign::Start, TextEllipsize::Middle);
      if (!sameMetrics(compact, again)) {
        std::println(stderr, "cairo_text_renderer_test: FAIL: middle ellipsis measurement not repeatable");
        return 1;
      }
      for (const auto mode : {TextEllipsize::Start, TextEllipsize::End}) {
        const auto normal = renderer.measure(scale, text, 14.0F, font, budget, 1, TextAlign::Start, mode);
        if (std::abs(normal.width - budget) >= 0.01F) {
          std::println(stderr, "cairo_text_renderer_test: FAIL: start/end ellipsis width at scale {}", scale);
          return 1;
        }
      }
      const auto spaces = renderer.measure(scale, "MMM MMM", 14.0F, font);
      const auto unchanged = renderer.measure(
          scale, "MMM MMM", 14.0F, font, spaces.width + 20.0F, 1, TextAlign::Start, TextEllipsize::Middle
      );
      if (!sameMetrics(spaces, unchanged)) {
        std::println(stderr, "cairo_text_renderer_test: FAIL: middle ellipsis changed untruncated text");
        return 1;
      }
    }
    return 0;
  }

} // namespace

int main() {
  CairoTextRenderer renderer;
  renderer.initialize(nullptr, nullptr);

  if (const int rc = scaleKeyedCacheCase(renderer); rc != 0) {
    return rc;
  }
  if (const int rc = wrappingBoundaryCase(renderer); rc != 0) {
    return rc;
  }
  return middleEllipsisSpacingCase(renderer);
}
