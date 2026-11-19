#pragma once

#include "shell/control_center/tab.h"
#include "ui/signal.h"

#include <array>
#include <string>

class Box;
class Flex;
class Glyph;
class Image;
class Label;
class ProgressBar;
class Renderer;
class ScreenTimeService;
class ConfigService;
class ScrollView;
class Segmented;
struct ScreenTimeSnapshot;

class ScreenTimeTab : public Tab {
public:
  ScreenTimeTab(ScreenTimeService* screenTime, ConfigService* config);

  std::unique_ptr<Flex> create() override;
  void onClose() override;
  void setActive(bool active) override;

private:
  void doLayout(Renderer& renderer, float contentWidth, float bodyHeight) override;
  void doUpdate(Renderer& renderer) override;
  void onPanelCardOpacityChanged(float opacity) override;
  void syncContent(Renderer& renderer);
  void syncChart(const ScreenTimeSnapshot& snapshot, float scale);
  void bindAppRowWidths(Renderer& renderer, float rowWidth);
  void layoutChart(Renderer& renderer);
  void layoutAppRows(Renderer& renderer);
  void syncEnabledUi();
  std::string resolveIconPath(const std::string& appKey) const;

  ScreenTimeService* m_screenTime = nullptr;
  ConfigService* m_config = nullptr;
  Signal<>::ScopedConnection m_appIconColorizeConn;
  bool m_active = false;
  int m_rangeDays = 1;
  std::string m_lastSnapshotKey;

  Flex* m_root = nullptr;
  Flex* m_statsCard = nullptr;
  Flex* m_appsCard = nullptr;
  Label* m_disabledLabel = nullptr;
  Segmented* m_rangePicker = nullptr;
  ScrollView* m_scroll = nullptr;
  Flex* m_chartPlotRow = nullptr;
  Flex* m_chartLabelRow = nullptr;
  Flex* m_appList = nullptr;
  Label* m_totalLabel = nullptr;
  Label* m_emptyLabel = nullptr;

  static constexpr int kMaxAppRows = 12;
  static constexpr int kMaxBuckets = 24;
  static constexpr float kChartHeight = 132.0F;
  static constexpr float kChartBarWidthRatio = 0.70F;
  static constexpr float kChartMinBarWidth = 10.0F;
  static constexpr float kChartGap = 2.0F;

  struct ChartColumnWidgets {
    Flex* column = nullptr;
    Flex* labelCell = nullptr;
    Box* fill = nullptr;
    Label* label = nullptr;
    float fillRatio = 0.0F;
  };
  std::array<ChartColumnWidgets, kMaxBuckets> m_chartColumns{};

  struct AppRowWidgets {
    Flex* row = nullptr;
    Flex* iconSlot = nullptr;
    Image* icon = nullptr;
    Glyph* iconFallback = nullptr;
    Label* name = nullptr;
    Label* duration = nullptr;
    ProgressBar* bar = nullptr;
    std::string iconPath;
  };
  std::array<AppRowWidgets, kMaxAppRows> m_appRows{};
  Signal<>::ScopedConnection m_paletteConn;

  void updateIconForRow(Renderer& renderer, AppRowWidgets& widgets, const std::string& appKey, float scale);
};
