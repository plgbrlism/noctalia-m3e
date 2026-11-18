#include "config/config_types.h"
#include "config/schema/config_schema.h"
#include "config/schema/diagnostics.h"
#include "config/toml.h"
#include "tests/test_check.h"

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

namespace {

  using Diagnostics = noctalia::config::schema::Diagnostics;

  constexpr std::string_view kParent = "motion";

  toml::table motionSection(std::string_view text) {
    const toml::table root = toml::parse(text);
    if (const toml::table* section = root["motion"].as_table()) {
      return *section;
    }
    return {};
  }

  bool hasDiagnostic(const Diagnostics& diag, std::string_view path, std::string_view message) {
    return std::ranges::any_of(diag.entries, [&](const Diagnostics::Entry& entry) {
      return entry.path.contains(path) && entry.message.contains(message);
    });
  }

} // namespace

int main() {
  const auto& fields = noctalia::config::schema::motionSchema();
  const auto& expressiveField = fields[0];
  const auto& panelField = fields[1];

  {
    Diagnostics diag;
    MotionUiConfig out;
    expressiveField.read(toml::table{}, out, kParent, diag);
    panelField.read(toml::table{}, out, kParent, diag);
    TEST_CHECK(out.expressive);
    TEST_CHECK(out.panel == PanelMotion::Directional);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionUiConfig out;
    const toml::table table = motionSection(R"(
[motion]
expressive = false
panel = "depth"
)");
    expressiveField.read(table, out, kParent, diag);
    panelField.read(table, out, kParent, diag);
    TEST_CHECK(!out.expressive);
    TEST_CHECK(out.panel == PanelMotion::Depth);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Diagnostics diag;
    MotionUiConfig out;
    const toml::table table = motionSection(R"(
[motion]
panel = "bogus"
)");
    panelField.read(table, out, kParent, diag);
    TEST_CHECK(out.panel == PanelMotion::Directional);
    TEST_CHECK(hasDiagnostic(diag, "panel", "unknown value"));
  }

  {
    Diagnostics diag;
    MotionUiConfig out;
    const toml::table table = motionSection(R"(
[motion]
expressive = "yes"
)");
    expressiveField.read(table, out, kParent, diag);
    TEST_CHECK(out.expressive);
    TEST_CHECK(hasDiagnostic(diag, "expressive", "expected a boolean"));
  }

  {
    MotionUiConfig original;
    original.expressive = false;
    original.panel = PanelMotion::Fluid;

    toml::table written;
    expressiveField.write(written, original);
    panelField.write(written, original);

    Diagnostics diag;
    MotionUiConfig roundTripped;
    expressiveField.read(written, roundTripped, kParent, diag);
    panelField.read(written, roundTripped, kParent, diag);
    TEST_CHECK(roundTripped == original);
    TEST_CHECK(diag.entries.empty());
  }

  return 0;
}
