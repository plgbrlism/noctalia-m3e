#include "config/schema/config_schema.h"
#include "config/schema/diagnostics.h"
#include "config/toml.h"
#include "render/animation/motion_scheme.h"
#include "tests/test_check.h"

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

  using Diagnostics = noctalia::config::schema::Diagnostics;

  constexpr std::string_view kParent = "motion";

  constexpr std::size_t slot(SpringToken token) { return static_cast<std::size_t>(token); }

  toml::table motionSection(std::string_view text) {
    const toml::table root = toml::parse(text);
    if (const toml::table* section = root["motion"].as_table()) {
      return *section;
    }
    return {};
  }

  bool hasError(const Diagnostics& diag, std::string_view path, std::string_view message) {
    return std::ranges::any_of(diag.entries, [&](const Diagnostics::Entry& entry) {
      return entry.severity == Diagnostics::Severity::Error
          && entry.path.contains(path)
          && entry.message.contains(message);
    });
  }

} // namespace

int main() {
  const auto& field = noctalia::config::schema::motionSchema()[0];

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(toml::table{}, out, kParent, diag);
    TEST_CHECK(out.scheme == MotionScheme::Expressive);
    TEST_CHECK(!out.reduce);
    TEST_CHECK(std::ranges::none_of(out.overrides, [](const auto& entry) { return entry.has_value(); }));
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion]
scheme = "standard"
)"),
        out, kParent, diag
    );
    TEST_CHECK(out.scheme == MotionScheme::Standard);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion]
scheme = "bogus"
)"),
        out, kParent, diag
    );
    TEST_CHECK(hasError(diag, "scheme", "standard"));
    TEST_CHECK(out.scheme == MotionScheme::Expressive);
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion.effects.fast]
damping = 1.2
stiffness = 800
)"),
        out, kParent, diag
    );
    const auto& spec = out.overrides[slot(SpringToken::EffectsFast)];
    TEST_CHECK(spec.has_value());
    TEST_NEAR(spec->dampingRatio, 1.2F, 1e-6F);
    TEST_NEAR(spec->stiffness, 800.0F, 1e-6F);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion.effects.fast]
damping = 0.9
stiffness = 800
)"),
        out, kParent, diag
    );
    TEST_CHECK(hasError(diag, "damping", "must be >= 1.0"));
    TEST_CHECK(!out.overrides[slot(SpringToken::EffectsFast)].has_value());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion.spatial.fast]
damping = 0
stiffness = 380
)"),
        out, kParent, diag
    );
    TEST_CHECK(hasError(diag, "damping", "must be > 0"));
    TEST_CHECK(!out.overrides[slot(SpringToken::SpatialFast)].has_value());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion.spatial.fast]
damping = 0.8
)"),
        out, kParent, diag
    );
    TEST_CHECK(hasError(diag, "stiffness", "expected a number"));
    TEST_CHECK(!out.overrides[slot(SpringToken::SpatialFast)].has_value());
  }

  {
    const toml::table table = motionSection(R"(
[motion.spatial.bogus]
damping = 0.8
stiffness = 380
)");
    std::vector<std::string> unknown;
    field.findUnknown(table, kParent, unknown);
    TEST_CHECK(std::ranges::find(unknown, std::string("motion.spatial.bogus")) != unknown.end());
  }

  {
    MotionConfig original;
    original.scheme = MotionScheme::Standard;
    original.overrides[slot(SpringToken::SpatialFast)] = SpringSpec{0.7F, 500.0F};

    toml::table written;
    field.write(written, original);

    Diagnostics diag;
    MotionConfig roundTripped;
    field.read(written, roundTripped, kParent, diag);
    TEST_CHECK(roundTripped == original);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionConfig out;
    field.read(
        motionSection(R"(
[motion]
reduce = true
)"),
        out, kParent, diag
    );
    TEST_CHECK(out.reduce);
  }

  {
    Diagnostics diag;
    MotionConfig out;
    out.scheme = MotionScheme::Standard;
    out.overrides[slot(SpringToken::SpatialFast)] = SpringSpec{0.9F, 1000.0F};

    field.read(
        motionSection(R"(
[motion]
scheme = "bogus"

[motion.effects.fast]
damping = 0.5
stiffness = 800
)"),
        out, kParent, diag
    );
    TEST_CHECK(out.scheme == MotionScheme::Standard);
    TEST_CHECK(out.overrides[slot(SpringToken::SpatialFast)].has_value());
    TEST_NEAR(out.overrides[slot(SpringToken::SpatialFast)]->dampingRatio, 0.9F, 1e-6F);
    TEST_NEAR(out.overrides[slot(SpringToken::SpatialFast)]->stiffness, 1000.0F, 1e-6F);
    TEST_CHECK(hasError(diag, "scheme", "standard"));
    TEST_CHECK(hasError(diag, "damping", "must be >= 1.0"));
  }

  return 0;
}
