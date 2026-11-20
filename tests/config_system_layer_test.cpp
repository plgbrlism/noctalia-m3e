#include "config/config_service.h"
#include "config/config_types.h"
#include "tests/test_check.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <unistd.h>

namespace {

  class TestRoot {
  public:
    explicit TestRoot(const std::string_view name)
        : m_root(std::filesystem::temp_directory_path() / (std::string(name) + "-" + std::to_string(::getpid()))) {
      std::filesystem::remove_all(m_root);
      std::filesystem::create_directories(userDir());
      std::filesystem::create_directories(m_root / "state" / "vespertine");
      std::filesystem::create_directories(m_root / "data");
      ::setenv("VESPERTINE_CONFIG_HOME", (m_root / "config").c_str(), 1);
      ::setenv("VESPERTINE_STATE_HOME", (m_root / "state").c_str(), 1);
      ::setenv("VESPERTINE_DATA_HOME", (m_root / "data").c_str(), 1);
    }

    ~TestRoot() {
      ::unsetenv("XDG_CONFIG_DIRS");
      ::unsetenv("VESPERTINE_CONFIG_HOME");
      ::unsetenv("VESPERTINE_STATE_HOME");
      ::unsetenv("VESPERTINE_DATA_HOME");
      std::filesystem::remove_all(m_root);
    }

    TestRoot(const TestRoot&) = delete;
    TestRoot& operator=(const TestRoot&) = delete;

    [[nodiscard]] std::filesystem::path base(const std::string_view name) const { return m_root / name; }
    [[nodiscard]] std::filesystem::path userDir() const { return m_root / "config" / "vespertine"; }

  private:
    std::filesystem::path m_root;
  };

  void setSystemBases(const std::string_view bases) { ::setenv("XDG_CONFIG_DIRS", std::string(bases).c_str(), 1); }

  void writeFile(const std::filesystem::path& path, const std::string_view content) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream out(path, std::ios::trunc);
    out << content;
  }

  void writeWallpaper(const std::filesystem::path& path, const std::string_view wallpaper) {
    writeFile(path, std::string("[wallpaper.default]\npath = \"") + std::string(wallpaper) + "\"\n");
  }

  void checkSystemLayerKeepsBuiltinDefaults() {
    const TestRoot root("vespertine-system-layer-defaults");
    setSystemBases(root.base("vendor").string());
    writeWallpaper(root.base("vendor") / "vespertine" / "10-vendor.toml", "/usr/share/backgrounds/vendor.png");

    const ConfigService config;
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/usr/share/backgrounds/vendor.png");
    TEST_CHECK(!config.config().bars.empty());
    TEST_CHECK(!config.config().idle.behaviors.empty());
    TEST_CHECK(!config.config().shell.session.actions.empty());
  }

  void checkFirstSystemBaseIsMostImportant() {
    const TestRoot root("vespertine-system-layer-order");
    setSystemBases(root.base("admin").string() + ":" + root.base("vendor").string());
    writeWallpaper(root.base("admin") / "vespertine" / "config.toml", "/admin.png");
    writeWallpaper(root.base("vendor") / "vespertine" / "config.toml", "/vendor.png");

    const ConfigService config;
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/admin.png");
  }

  void checkRepeatedSystemBaseKeepsFirstPosition() {
    const TestRoot root("vespertine-system-layer-repeat");
    const std::string admin = root.base("admin").string();
    setSystemBases(admin + ":" + root.base("vendor").string() + ":" + admin + "/");
    writeWallpaper(root.base("admin") / "vespertine" / "config.toml", "/admin.png");
    writeWallpaper(root.base("vendor") / "vespertine" / "config.toml", "/vendor.png");

    const ConfigService config;
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/admin.png");
  }

  void checkUserAndSettingsOutrankSystem() {
    const TestRoot root("vespertine-system-layer-precedence");
    setSystemBases(root.base("vendor").string());
    writeWallpaper(root.base("vendor") / "vespertine" / "config.toml", "/vendor.png");
    writeWallpaper(root.userDir() / "config.toml", "/user.png");

    ConfigService config;
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/user.png");
    config.setWallpaperPath(std::nullopt, "/picked.png");
    config.forceReload();
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/picked.png");
  }

  void checkUserAutoloadOptOutKeepsSystemLayer() {
    const TestRoot root("vespertine-system-layer-autoload");
    setSystemBases(root.base("vendor").string());
    writeFile(root.base("vendor") / "vespertine" / "config.toml", "[theme]\nmode = \"light\"\n");
    writeFile(root.userDir() / "config.toml", "[include]\nautoload = false\n");
    writeWallpaper(root.userDir() / "other.toml", "/skipped.png");

    const ConfigService config;
    TEST_CHECK(config.config().theme.mode == ThemeMode::Light);
    TEST_CHECK(config.getWallpaperPath("DP-1") != "/skipped.png");
  }

  void checkUserDirListedAsSystemBaseKeepsUserPrecedence() {
    const TestRoot root("vespertine-system-layer-user-dir");
    setSystemBases(root.base("vendor").string() + ":" + root.userDir().parent_path().string());
    writeWallpaper(root.base("vendor") / "vespertine" / "config.toml", "/vendor.png");
    writeWallpaper(root.userDir() / "config.toml", "/user.png");

    const ConfigService config;
    TEST_CHECK(config.getWallpaperPath("DP-1") == "/user.png");
  }

} // namespace

int main(const int argc, char** argv) {
  TEST_CHECK(argc == 2);
  ::setenv("VESPERTINE_ASSETS_DIR", argv[1], 1);
  checkSystemLayerKeepsBuiltinDefaults();
  checkFirstSystemBaseIsMostImportant();
  checkRepeatedSystemBaseKeepsFirstPosition();
  checkUserAndSettingsOutrankSystem();
  checkUserAutoloadOptOutKeepsSystemLayer();
  checkUserDirListedAsSystemBaseKeepsUserPrecedence();
  return 0;
}
