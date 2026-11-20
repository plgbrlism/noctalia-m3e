#include "scripting/luau_host.h"
#include "scripting/script_api_context.h"
#include "system/desktop_entry.h"
#include "tests/test_check.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <string_view>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

  void writeDesktopEntry(
      const fs::path& dir, std::string_view id, std::string_view name, std::string_view icon, std::string_view wmClass
  ) {
    std::ofstream(dir / (std::string(id) + ".desktop"))
        << "[Desktop Entry]\n"
        << "Type=Application\n"
        << "Name="
        << name
        << "\n"
        << "Exec="
        << id
        << "\n"
        << "Icon="
        << icon
        << "\n"
        << "StartupWMClass="
        << wmClass
        << "\n";
  }

} // namespace

int main() {
  char tempDir[] = "/tmp/vespertine-app-icon-path-test-XXXXXX";
  if (mkdtemp(tempDir) == nullptr) {
    std::perror("mkdtemp");
    return 1;
  }
  const fs::path root(tempDir);
  const fs::path data = root / "data";
  const fs::path applications = data / "applications";
  const fs::path pixmaps = data / "pixmaps";
  fs::create_directories(applications);
  fs::create_directories(pixmaps);

  // Shaped like com.microsoft.VSCode / com.microsoft.VSCodeInsiders (issue #5250), with made-up names
  // because the scan always appends /usr/share and host entries or themed icons must not interfere.
  writeDesktopEntry(
      applications, "org.example.VespertineSample", "Vespertine Sample", "vespertinesample", "org.example.VespertineSample"
  );
  writeDesktopEntry(
      applications, "org.example.VespertineSampleInsiders", "Vespertine Sample - Insiders", "vespertinesample-insiders",
      "org.example.VespertineSampleInsiders"
  );
  writeDesktopEntry(applications, "org.example.vespertinewrapped", "Wrapped Sample", "org.example.vespertinewrapped", "");
  for (const char* icon : {"vespertinesample.png", "vespertinesample-insiders.png", "org.example.vespertinewrapped.png"}) {
    std::ofstream(pixmaps / icon) << "png";
  }

  setenv("HOME", tempDir, 1);
  setenv("XDG_DATA_HOME", (root / "empty").c_str(), 1);
  setenv("XDG_DATA_DIRS", data.c_str(), 1);
  // Populates the snapshot the binding reads.
  TEST_CHECK(
      std::ranges::count_if(
          desktopEntries(), [](const DesktopEntry& entry) { return entry.id.starts_with("org.example."); }
      )
      == 3
  );

  scripting::ScriptApiContext api;
  LuauHost host(api, "test/plugin:service");
  const std::string px = pixmaps.string() + "/";
  TEST_CHECK(host.exec(
      "=app-icon-path",
      std::format(
          R"(
    local px = "{}"
    -- Raw icon name: must not be token-matched to org.example.VespertineSample.
    assert(vespertine.appIconPath("vespertinesample-insiders") == px .. "vespertinesample-insiders.png")
    -- Absolute paths are returned as-is when they exist, nil otherwise.
    assert(vespertine.appIconPath(px .. "vespertinesample-insiders.png") == px .. "vespertinesample-insiders.png")
    assert(vespertine.appIconPath(px .. "missing-vespertinesample.png") == nil)
    -- Exact desktop-entry ids and raw names keep working.
    assert(vespertine.appIconPath("org.example.VespertineSampleInsiders") == px .. "vespertinesample-insiders.png")
    assert(vespertine.appIconPath("vespertinesample") == px .. "vespertinesample.png")
    -- Window app ids still reach the taskbar heuristics (#4571).
    assert(vespertine.appIconPath(".vespertinewrapped-gui-wrapped") == px .. "org.example.vespertinewrapped.png")
    -- No bare "vespertine" token: an installed dev.vespertine.Vespertine entry would token-match it.
    assert(vespertine.appIconPath("vespertinemissing-app") == nil)
  )",
          px
      )
  ));

  std::error_code ec;
  fs::remove_all(root, ec);
  return 0;
}
