#include "render/animation/motion_scheme.h"
#include "tests/test_check.h"

#include <array>

namespace {

  struct ExpectedSpring {
    MotionScheme scheme;
    SpringToken token;
    float dampingRatio;
    float stiffness;
  };

  constexpr std::array<SpringToken, 6> kTokens = {
    SpringToken::SpatialFast,
    SpringToken::SpatialDefault,
    SpringToken::SpatialSlow,
    SpringToken::EffectsFast,
    SpringToken::EffectsDefault,
    SpringToken::EffectsSlow,
  };

  constexpr std::array<SpringToken, 3> kSpatialTokens = {
    SpringToken::SpatialFast,
    SpringToken::SpatialDefault,
    SpringToken::SpatialSlow,
  };

  constexpr std::array<SpringToken, 3> kEffectsTokens = {
    SpringToken::EffectsFast,
    SpringToken::EffectsDefault,
    SpringToken::EffectsSlow,
  };

  constexpr std::array<ExpectedSpring, 12> kExpectedSprings = {{
    {MotionScheme::Standard, SpringToken::SpatialFast, 0.9F, 1400.0F},
    {MotionScheme::Standard, SpringToken::SpatialDefault, 0.9F, 700.0F},
    {MotionScheme::Standard, SpringToken::SpatialSlow, 0.9F, 300.0F},
    {MotionScheme::Standard, SpringToken::EffectsFast, 1.0F, 3800.0F},
    {MotionScheme::Standard, SpringToken::EffectsDefault, 1.0F, 1600.0F},
    {MotionScheme::Standard, SpringToken::EffectsSlow, 1.0F, 800.0F},
    {MotionScheme::Expressive, SpringToken::SpatialFast, 0.6F, 800.0F},
    {MotionScheme::Expressive, SpringToken::SpatialDefault, 0.8F, 380.0F},
    {MotionScheme::Expressive, SpringToken::SpatialSlow, 0.8F, 200.0F},
    {MotionScheme::Expressive, SpringToken::EffectsFast, 1.0F, 3800.0F},
    {MotionScheme::Expressive, SpringToken::EffectsDefault, 1.0F, 1600.0F},
    {MotionScheme::Expressive, SpringToken::EffectsSlow, 1.0F, 800.0F},
  }};

} // namespace

int main() {
  constexpr float kEps = 0.0001F;

  {
    const SpringTokenResolver resolver;
    TEST_CHECK(kDefaultMotionScheme == MotionScheme::Expressive);
    TEST_CHECK(resolver.scheme() == MotionScheme::Expressive);
  }

  {
    SpringTokenResolver resolver;
    for (const auto& expected : kExpectedSprings) {
      resolver.setScheme(expected.scheme);
      const auto actual = resolver.resolve(expected.token);
      TEST_NEAR(actual.dampingRatio, expected.dampingRatio, kEps);
      TEST_NEAR(actual.stiffness, expected.stiffness, kEps);
    }
  }

  {
    const auto token = SpringTokenResolver::parseConfigKey("spatial.fast");
    TEST_CHECK(token.has_value());
    TEST_CHECK(*token == SpringToken::SpatialFast);
    TEST_CHECK(!SpringTokenResolver::parseConfigKey("spatial.unknown").has_value());
  }

  {
    const auto standard = SpringTokenResolver::parseScheme("standard");
    TEST_CHECK(standard.has_value());
    TEST_CHECK(*standard == MotionScheme::Standard);

    const auto expressive = SpringTokenResolver::parseScheme("expressive");
    TEST_CHECK(expressive.has_value());
    TEST_CHECK(*expressive == MotionScheme::Expressive);

    TEST_CHECK(!SpringTokenResolver::parseScheme("unknown").has_value());
  }

  for (const auto token : kTokens) {
    const auto parsed = SpringTokenResolver::parseConfigKey(SpringTokenResolver::toString(token));
    TEST_CHECK(parsed.has_value());
    TEST_CHECK(*parsed == token);
  }

  for (const auto token : kSpatialTokens) {
    const auto standardId = SpringTokenResolver::toInternalId(MotionScheme::Standard, token);
    const auto expressiveId = SpringTokenResolver::toInternalId(MotionScheme::Expressive, token);
    TEST_CHECK(standardId != expressiveId);
  }

  TEST_CHECK(
      SpringTokenResolver::toInternalId(MotionScheme::Expressive, SpringToken::SpatialFast)
      == "M3E-EXPRESSIVE-SPATIAL-FAST"
  );

  {
    const SpringTokenResolver resolver;
    TEST_CHECK(!resolver.tryResolve("spatial.unknown").has_value());

    const auto fast = resolver.tryResolve("spatial.fast");
    TEST_CHECK(fast.has_value());
    TEST_NEAR(fast->dampingRatio, 0.6F, kEps);
    TEST_NEAR(fast->stiffness, 800.0F, kEps);
  }

  {
    const SpringTokenResolver standard(MotionScheme::Standard);
    const SpringTokenResolver expressive(MotionScheme::Expressive);

    for (const auto token : kSpatialTokens) {
      const auto standardSpec = standard.resolve(token);
      const auto expressiveSpec = expressive.resolve(token);
      TEST_CHECK(
          standardSpec.dampingRatio != expressiveSpec.dampingRatio
          || standardSpec.stiffness != expressiveSpec.stiffness
      );
    }

    for (const auto token : kEffectsTokens) {
      const auto standardSpec = standard.resolve(token);
      const auto expressiveSpec = expressive.resolve(token);
      TEST_NEAR(standardSpec.dampingRatio, expressiveSpec.dampingRatio, kEps);
      TEST_NEAR(standardSpec.stiffness, expressiveSpec.stiffness, kEps);
    }
  }

  {
    const SpringTokenResolver baseline;
    const auto spec = baseline.resolve(SpringToken::SpatialFast);
    TEST_NEAR(spec.dampingRatio, 0.6F, kEps);
    TEST_NEAR(spec.stiffness, 800.0F, kEps);
  }

  // TODO(MOT-006/CHO-001): choreography disable assertion deferred —
  // CHO-001/002 ship in P3. MotionConfig::reduce is queryable now;
  // verify choreography suppression when CHO lands.

  {
    auto makeReduceConfig = [](MotionScheme scheme, bool reduce) {
      MotionConfig c;
      c.scheme = scheme;
      c.reduce = reduce;
      return c;
    };

    {
      const SpringTokenResolver r(makeReduceConfig(MotionScheme::Expressive, true));
      const auto spec = r.resolve(SpringToken::SpatialFast);
      TEST_NEAR(spec.dampingRatio, 1.0F, kEps);
      TEST_NEAR(spec.stiffness, 800.0F, kEps);
    }

    {
      const SpringTokenResolver r(makeReduceConfig(MotionScheme::Standard, true));
      const auto spec = r.resolve(SpringToken::SpatialFast);
      TEST_NEAR(spec.dampingRatio, 1.0F, kEps);
      TEST_NEAR(spec.stiffness, 1400.0F, kEps);
    }

    {
      MotionConfig c;
      c.scheme = MotionScheme::Expressive;
      c.reduce = true;
      c.overrides[static_cast<std::size_t>(SpringToken::SpatialFast)] = SpringSpec{0.3F, 999.0F};
      const SpringTokenResolver r(c);
      const auto spec = r.resolve(SpringToken::SpatialFast);
      TEST_NEAR(spec.dampingRatio, 1.0F, kEps);
      TEST_NEAR(spec.stiffness, 999.0F, kEps);
    }

    {
      const SpringTokenResolver r(makeReduceConfig(MotionScheme::Expressive, true));
      const auto spec = r.resolve(SpringToken::EffectsDefault);
      TEST_NEAR(spec.dampingRatio, 1.0F, kEps);
      TEST_NEAR(spec.stiffness, 1600.0F, kEps);
    }

    {
      const SpringTokenResolver r(makeReduceConfig(MotionScheme::Expressive, false));
      const auto spec = r.resolve(SpringToken::SpatialFast);
      TEST_NEAR(spec.dampingRatio, 0.6F, kEps);
      TEST_NEAR(spec.stiffness, 800.0F, kEps);
    }
  }

  return 0;
}
