#include "config/config_types.h"
#include "config/schema/config_schema.h"
#include "config/schema/diagnostics.h"
#include "config/schema/engine.h"
#include "config/toml.h"
#include "tests/test_check.h"

#include <string_view>

namespace {

  using noctalia::config::schema::Diagnostics;
  using noctalia::config::schema::readInto;
  using noctalia::config::schema::shellSchema;
  using noctalia::config::schema::writeTable;

  constexpr std::string_view kParent = "shell";

  toml::table shellSection(std::string_view text) {
    const toml::table root = toml::parse(text);
    if (const toml::table* section = root["shell"].as_table()) {
      return *section;
    }
    return {};
  }

} // namespace

int main() {
  {
    ShellConfig cfg{};
    TEST_CHECK(cfg.tooltip.border);
  }

  {
    Diagnostics diag;
    ShellConfig out;
    readInto(
        shellSection(R"(
[shell.tooltip]
border = false
)"),
        out, shellSchema(), kParent, diag
    );
    TEST_CHECK(!out.tooltip.border);
    TEST_CHECK(diag.entries.empty());
  }

  for (const bool value : {true, false}) {
    ShellConfig original{};
    original.tooltip.border = value;

    const toml::table written = writeTable(original, shellSchema());

    Diagnostics diag;
    ShellConfig roundTripped;
    readInto(written, roundTripped, shellSchema(), kParent, diag);
    TEST_CHECK(roundTripped.tooltip.border == value);
    TEST_CHECK(diag.entries.empty());
  }

  {
    Config prev{};
    Config next{};
    next.shell.tooltip.border = false;
    TEST_CHECK(computeConfigChangeSet(prev, next).shell);
  }

  return 0;
}
