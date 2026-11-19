#include "shell/control_center/tabs/screen_time_tab.h"

#include "config/config_service.h"
#include "i18n/i18n.h"
#include "render/core/renderer.h"
#include "render/scene/tooltip_content.h"
#include "shell/app_icon_colorization_settings.h"
#include "shell/panel/panel_manager.h"
#include "system/app_identity.h"
#include "system/desktop_entry.h"
#include "system/icon_resolver.h"
#include "system/internal_app_metadata.h"
#include "system/screen_time_service.h"
#include "time/time_format.h"
#include "ui/builders.h"
#include "ui/controls/image.h"
#include "ui/controls/progress_bar.h"
#include "ui/controls/scroll_view.h"
#include "ui/palette.h"
#include "ui/style.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace control_center;

namespace {

  constexpr float kAppIconSize = 28.0F;
  constexpr float kUsageBarHeight = 10.0F;

  IconResolver& screenTimeIconResolver() {
    static IconResolver resolver;
    return resolver;
  }

  std::string snapshotKey(int rangeDays, const ScreenTimeSnapshot& snapshot) {
    std::string key = std::to_string(rangeDays) + '|' + std::to_string(snapshot.total.count());
    key += snapshot.hourlyBuckets ? "|h" : "|d";
    for (const auto& bucket : snapshot.buckets) {
      key += ':';
      key += std::to_string(bucket.count());
    }
    for (const auto& label : snapshot.bucketLabels) {
      key += '~';
      key += label;
    }
    for (const auto& app : snapshot.apps) {
      key += '|';
      key += app.appKey;
      key += '@';
      key += std::to_string(app.total.count());
    }
    return key;
  }

  std::vector<TooltipRow> appUsageTooltip(const std::string& displayName, std::chrono::seconds duration) {
    return {{displayName, formatDuration(duration)}};
  }

} // namespace

ScreenTimeTab::ScreenTimeTab(ScreenTimeService* screenTime, ConfigService* config)
    : m_screenTime(screenTime), m_config(config) {
  m_appIconColorizeConn =
      shellAppIconColorizationChanged().connect([] { PanelManager::instance().requestUpdateOnly(); });
}

std::unique_ptr<Flex> ScreenTimeTab::create() {
  const float scale = contentScale();

  auto tab = ui::column({
      .out = &m_root,
      .align = FlexAlign::Stretch,
      .gap = Style::spaceSm * scale,
      .configure = [](Flex& column) { column.setClipChildren(true); },
  });

  m_rangeDays = 1;
  tab->addChild(
      ui::segmented({
          .out = &m_rangePicker,
          .options =
              std::vector<ui::SegmentedOption>{
                  {.label = i18n::tr("control-center.screen-time.range.today")},
                  {.label = i18n::tr("control-center.screen-time.range.3-days")},
                  {.label = i18n::tr("control-center.screen-time.range.14-days")},
              },
          .selectedIndex = 0,
          .fontSize = Style::fontSizeCaption,
          .scale = scale,
          .surfaceOpacity = panelCardOpacity(),
          .equalSegmentWidths = true,
          .onChange = [this](std::size_t idx) {
            static constexpr int kRanges[] = {1, 3, 14};
            m_rangeDays = kRanges[std::min(idx, std::size_t{2})];
            m_lastSnapshotKey.clear();
            PanelManager::instance().refresh();
          },
      })
  );

  auto scroll = ui::scrollView({
      .out = &m_scroll,
      .contentScale = scale,
      .scrollbarVisible = true,
      .flexGrow = 1.0F,
      .configure = [](ScrollView& scrollView) {
        scrollView.clearFill();
        scrollView.clearBorder();
      },
  });

  auto* content = scroll->content();
  content->setDirection(FlexDirection::Vertical);
  content->setAlign(FlexAlign::Stretch);
  content->setGap(Style::spaceMd * scale);

  auto statsCard = ui::column({
      .out = &m_statsCard,
      .gap = Style::spaceSm * scale,
      .configure = [scale, opacity = panelCardOpacity()](Flex& card) { applySectionCardStyle(card, scale, opacity); },
  });

  statsCard->addChild(
      ui::label({
          .out = &m_disabledLabel,
          .text = i18n::tr("control-center.screen-time.disabled"),
          .fontSize = Style::fontSizeBody * scale,
          .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
          .visible = false,
      })
  );

  statsCard->addChild(
      ui::label({
          .out = &m_totalLabel,
          .fontSize = Style::fontSizeHeader * 1.6F * scale,
          .fontWeight = FontWeight::Bold,
          .color = colorSpecFromRole(ColorRole::OnSurface),
      })
  );

  auto chartPlotRow = ui::row({
      .out = &m_chartPlotRow,
      .align = FlexAlign::Stretch,
      .justify = FlexJustify::Start,
      .gap = kChartGap * scale,
      .minHeight = kChartHeight * scale,
      .maxHeight = kChartHeight * scale,
      .fillWidth = true,
  });

  auto chartLabelRow = ui::row({
      .out = &m_chartLabelRow,
      .align = FlexAlign::Center,
      .justify = FlexJustify::Start,
      .gap = kChartGap * scale,
      .fillWidth = true,
  });

  for (std::size_t i = 0; i < m_chartColumns.size(); ++i) {
    auto& widgets = m_chartColumns[i];
    auto plotColumn = ui::column({
        .out = &widgets.column,
        .align = FlexAlign::Stretch,
        .justify = FlexJustify::End,
        .minHeight = kChartHeight * scale,
        .maxHeight = kChartHeight * scale,
        .visible = false,
    });

    plotColumn->addChild(
        ui::box({
            .out = &widgets.fill,
            .fill = colorSpecFromRole(ColorRole::Primary),
            .visible = false,
            .participatesInLayout = false,
        })
    );

    chartPlotRow->addChild(std::move(plotColumn));

    auto labelCell = ui::row(
        {.out = &widgets.labelCell,
         .align = FlexAlign::Center,
         .justify = FlexJustify::Center,
         .minHeight = Style::fontSizeCaption * scale * 1.8F,
         .visible = false},
        ui::label({
            .out = &widgets.label,
            .fontSize = Style::fontSizeCaption * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
            .maxLines = 1,
            .textAlign = TextAlign::Center,
            .visible = false,
            .participatesInLayout = false,
        })
    );
    chartLabelRow->addChild(std::move(labelCell));
  }

  statsCard->addChild(std::move(chartPlotRow));
  statsCard->addChild(std::move(chartLabelRow));
  content->addChild(std::move(statsCard));

  auto appsCard = ui::column({
      .out = &m_appsCard,
      .gap = Style::spaceSm * scale,
      .configure = [scale, opacity = panelCardOpacity()](Flex& card) { applySectionCardStyle(card, scale, opacity); },
  });

  appsCard->addChild(
      ui::label({
          .text = i18n::tr("control-center.screen-time.most-used"),
          .fontSize = Style::fontSizeCaption * scale,
          .fontWeight = FontWeight::Bold,
          .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
      })
  );

  auto appList = ui::column({
      .out = &m_appList,
      .align = FlexAlign::Stretch,
      .gap = Style::spaceMd * scale,
      .fillWidth = true,
  });

  for (std::size_t i = 0; i < m_appRows.size(); ++i) {
    auto row = ui::row({
        .out = &m_appRows[i].row,
        .align = FlexAlign::Center,
        .gap = Style::spaceSm * scale,
        .fillWidth = true,
        .visible = false,
    });

    auto iconSlot = ui::row(
        {.out = &m_appRows[i].iconSlot,
         .align = FlexAlign::Center,
         .justify = FlexJustify::Center,
         .width = kAppIconSize * scale,
         .height = kAppIconSize * scale},
        ui::image({
            .out = &m_appRows[i].icon,
            .fit = ImageFit::Cover,
            .radius = Style::scaledRadiusMd(scale),
            .width = kAppIconSize * scale,
            .height = kAppIconSize * scale,
            .visible = false,
            .participatesInLayout = false,
        }),
        ui::glyph({
            .out = &m_appRows[i].iconFallback,
            .glyph = "app-window",
            .glyphSize = kAppIconSize * 0.55F * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
            .participatesInLayout = false,
        })
    );

    row->addChild(std::move(iconSlot));
    row->addChild(
        ui::label({
            .out = &m_appRows[i].name,
            .fontSize = Style::fontSizeBody * scale,
            .color = colorSpecFromRole(ColorRole::OnSurface),
            .maxLines = 1,
            .ellipsize = TextEllipsize::End,
        })
    );
    row->addChild(
        ui::progressBar({
            .out = &m_appRows[i].bar,
            .fill = colorSpecFromRole(ColorRole::Primary),
            .track = colorSpecFromRole(ColorRole::SurfaceContainerHighest),
            .radius = kUsageBarHeight * 0.5F * scale,
            .progress = 0.0F,
            .height = kUsageBarHeight * scale,
            .flexGrow = 1.0F,
        })
    );
    row->addChild(
        ui::label({
            .out = &m_appRows[i].duration,
            .fontSize = Style::fontSizeCaption * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
            .maxLines = 1,
            .textAlign = TextAlign::End,
        })
    );

    appList->addChild(std::move(row));
  }

  appList->addChild(
      ui::label({
          .out = &m_emptyLabel,
          .text = i18n::tr("control-center.screen-time.empty"),
          .fontSize = Style::fontSizeBody * scale,
          .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
      })
  );

  appsCard->addChild(std::move(appList));
  content->addChild(std::move(appsCard));
  tab->addChild(std::move(scroll));
  syncEnabledUi();
  m_paletteConn = paletteChanged().connect([this] {
    m_lastSnapshotKey.clear();
    PanelManager::instance().requestUpdateOnly();
  });
  return tab;
}

void ScreenTimeTab::onClose() {
  m_root = nullptr;
  m_statsCard = nullptr;
  m_appsCard = nullptr;
  m_disabledLabel = nullptr;
  m_rangePicker = nullptr;
  m_scroll = nullptr;
  m_chartPlotRow = nullptr;
  m_chartLabelRow = nullptr;
  m_appList = nullptr;
  m_totalLabel = nullptr;
  m_emptyLabel = nullptr;
  m_chartColumns = {};
  m_appRows = {};
  m_lastSnapshotKey.clear();
  m_rangeDays = 1;
  m_paletteConn = {};
}

void ScreenTimeTab::setActive(bool active) {
  m_active = active;
  if (m_active) {
    m_lastSnapshotKey.clear();
    syncEnabledUi();
  }
}

void ScreenTimeTab::syncEnabledUi() {
  const bool enabled = m_screenTime != nullptr && m_screenTime->enabled();
  if (m_disabledLabel != nullptr) {
    m_disabledLabel->setVisible(!enabled);
  }
  if (m_rangePicker != nullptr) {
    m_rangePicker->setVisible(enabled);
  }
  if (m_totalLabel != nullptr) {
    m_totalLabel->setVisible(enabled);
  }
  if (m_chartPlotRow != nullptr) {
    m_chartPlotRow->setVisible(enabled);
  }
  if (m_chartLabelRow != nullptr) {
    m_chartLabelRow->setVisible(enabled);
  }
  if (m_appsCard != nullptr) {
    m_appsCard->setVisible(enabled);
  }
  if (m_appList != nullptr) {
    m_appList->setVisible(enabled);
  }
}

void ScreenTimeTab::doLayout(Renderer& renderer, float contentWidth, float bodyHeight) {
  if (m_root == nullptr) {
    return;
  }

  bindAppRowWidths(renderer, contentWidth);
  m_root->setSize(contentWidth, bodyHeight);
  m_root->layout(renderer);
  layoutChart(renderer);
  layoutAppRows(renderer);
}

void ScreenTimeTab::doUpdate(Renderer& renderer) {
  if (!m_active) {
    return;
  }
  const auto tint = m_config != nullptr ? effectiveShellAppIconColorizationTint(
                                              m_config->config().shell, m_config->config().shell.screenTimeColorizeIcons
                                          )
                                        : std::nullopt;
  for (auto& row : m_appRows) {
    if (row.icon != nullptr) {
      row.icon->setAppIconColorization(tint);
    }
  }

  syncContent(renderer);
  bindAppRowWidths(renderer, m_root != nullptr ? m_root->width() : 0.0F);
  if (m_root != nullptr) {
    m_root->layout(renderer);
  }
  layoutChart(renderer);
  layoutAppRows(renderer);
}

void ScreenTimeTab::syncChart(const ScreenTimeSnapshot& snapshot, float scale) {
  const std::size_t bucketCount = std::min(snapshot.buckets.size(), m_chartColumns.size());
  std::chrono::seconds maxBucket{0};
  for (std::size_t i = 0; i < bucketCount; ++i) {
    maxBucket = std::max(maxBucket, snapshot.buckets[i]);
  }

  for (std::size_t i = 0; i < m_chartColumns.size(); ++i) {
    auto& widgets = m_chartColumns[i];
    const bool active = i < bucketCount;
    if (widgets.column != nullptr) {
      widgets.column->setVisible(active);
    }
    if (widgets.labelCell != nullptr) {
      widgets.labelCell->setVisible(active);
    }
    if (!active) {
      widgets.fillRatio = 0.0F;
      if (widgets.label != nullptr) {
        widgets.label->setText(std::string{});
        widgets.label->setVisible(false);
      }
      continue;
    }

    widgets.fillRatio = maxBucket.count() > 0
        ? static_cast<float>(snapshot.buckets[i].count()) / static_cast<float>(maxBucket.count())
        : 0.0F;

    if (widgets.label == nullptr) {
      continue;
    }

    widgets.label->setFontSize(Style::fontSizeCaption * scale);
    std::string label;
    if (snapshot.hourlyBuckets) {
      if (i == 0) {
        label = i18n::tr("control-center.screen-time.hour-0");
      } else if (i == 6) {
        label = i18n::tr("control-center.screen-time.hour-6");
      } else if (i == 12) {
        label = i18n::tr("control-center.screen-time.hour-12");
      } else if (i == 18) {
        label = i18n::tr("control-center.screen-time.hour-18");
      }
    } else if (bucketCount <= 3 || i % 4 == 0 || i + 1 == bucketCount) {
      if (i < snapshot.bucketLabels.size()) {
        label = snapshot.bucketLabels[i];
      }
    }
    widgets.label->setText(label);
    widgets.label->setVisible(!label.empty());
  }

  PanelManager::instance().requestRedraw();
}

void ScreenTimeTab::bindAppRowWidths(Renderer& renderer, float rowWidth) {
  if (rowWidth <= 0.0F) {
    return;
  }

  const float scale = contentScale();
  const float cardPadding = Style::spaceMd * scale;
  const float innerWidth = std::max(1.0F, rowWidth - 2.0F * cardPadding);
  const float rowGap = Style::spaceSm * scale;
  const float minBarWidth = 64.0F * scale;

  float maxDurationWidth = 0.0F;
  float maxNameWidth = 0.0F;
  for (auto& widgets : m_appRows) {
    if (widgets.row == nullptr || !widgets.row->visible()) {
      continue;
    }
    if (widgets.duration != nullptr) {
      widgets.duration->setMinWidth(0.0F);
      widgets.duration->measure(renderer);
      maxDurationWidth = std::max(maxDurationWidth, widgets.duration->width());
    }
    if (widgets.name != nullptr) {
      widgets.name->setMinWidth(0.0F);
      widgets.name->setMaxWidth(0.0F);
      widgets.name->measure(renderer);
      maxNameWidth = std::max(maxNameWidth, widgets.name->width());
    }
  }

  maxDurationWidth = std::ceil(maxDurationWidth);
  const float availableForName = std::max(
      1.0F,
      innerWidth - kAppIconSize * scale - maxDurationWidth - rowGap * 3.0F - minBarWidth
  );
  const float nameWidth = std::min(maxNameWidth, std::min(availableForName, innerWidth * 0.42F));

  for (auto& widgets : m_appRows) {
    if (widgets.row == nullptr || !widgets.row->visible()) {
      continue;
    }
    if (widgets.duration != nullptr) {
      widgets.duration->setMinWidth(maxDurationWidth);
    }
    if (widgets.name != nullptr) {
      widgets.name->setMinWidth(nameWidth);
      widgets.name->setMaxWidth(nameWidth);
    }
  }
}

void ScreenTimeTab::syncContent(Renderer& renderer) {
  if (m_screenTime == nullptr || m_totalLabel == nullptr || m_appList == nullptr) {
    return;
  }

  syncEnabledUi();
  if (!m_screenTime->enabled()) {
    m_lastSnapshotKey.clear();
    return;
  }

  ScreenTimeSnapshot snapshot = m_screenTime->snapshot(m_rangeDays);
  const std::string key = snapshotKey(m_rangeDays, snapshot);
  if (key == m_lastSnapshotKey) {
    return;
  }
  m_lastSnapshotKey = key;

  m_totalLabel->setText(formatDuration(snapshot.total));
  syncChart(snapshot, contentScale());

  const bool hasApps = !snapshot.apps.empty();
  if (m_emptyLabel != nullptr) {
    m_emptyLabel->setVisible(!hasApps);
  }

  std::chrono::seconds topTotal{0};
  if (hasApps) {
    topTotal = std::max_element(
                  snapshot.apps.begin(),
                  snapshot.apps.end(),
                  [](const auto& a, const auto& b) { return a.total < b.total; }
    )->total;
  }

  const float scale = contentScale();
  for (std::size_t i = 0; i < m_appRows.size(); ++i) {
    auto& widgets = m_appRows[i];
    if (widgets.row == nullptr) {
      continue;
    }
    if (i >= snapshot.apps.size()) {
      widgets.row->setVisible(false);
      if (widgets.duration != nullptr) {
        widgets.duration->clearTooltip();
      }
      if (widgets.name != nullptr) {
        widgets.name->clearTooltip();
      }
      continue;
    }

    const auto& app = snapshot.apps[i];
    widgets.row->setVisible(true);
    if (widgets.name != nullptr) {
      widgets.name->setText(app.displayName);
      widgets.name->setTooltip(appUsageTooltip(app.displayName, app.total));
    }
    if (widgets.duration != nullptr) {
      widgets.duration->setText(formatDuration(app.total));
      widgets.duration->setTooltip(appUsageTooltip(app.displayName, app.total));
    }
    if (widgets.bar != nullptr) {
      const float ratio =
          topTotal.count() > 0 ? static_cast<float>(app.total.count()) / static_cast<float>(topTotal.count()) : 0.0F;
      widgets.bar->setProgress(std::clamp(ratio, 0.0F, 1.0F));
    }

    updateIconForRow(renderer, widgets, app.appKey, scale);
  }
}

void ScreenTimeTab::layoutChart(Renderer& renderer) {
  if (m_chartPlotRow == nullptr || !m_chartPlotRow->visible()) {
    return;
  }

  const float scale = contentScale();
  const float chartHeight = kChartHeight * scale;
  const float gap = kChartGap * scale;

  std::size_t activeBuckets = 0;
  for (const auto& widgets : m_chartColumns) {
    if (widgets.column != nullptr && widgets.column->visible()) {
      activeBuckets++;
    }
  }
  if (activeBuckets == 0) {
    return;
  }

  float rowWidth = m_chartPlotRow->width();
  if (rowWidth <= 0.0F && m_statsCard != nullptr) {
    m_statsCard->layout(renderer);
    rowWidth = m_chartPlotRow->width();
  }
  if (rowWidth <= 0.0F) {
    return;
  }

  const float slotWidth =
      (rowWidth - gap * static_cast<float>(activeBuckets - 1)) / static_cast<float>(activeBuckets);
  if (slotWidth <= 0.0F) {
    return;
  }
  const float barWidth =
      std::min(slotWidth, std::max(kChartMinBarWidth * scale, slotWidth * kChartBarWidthRatio));
  const float barRadius = barWidth * 0.5F;

  for (auto& widgets : m_chartColumns) {
    if (widgets.column == nullptr || !widgets.column->visible()) {
      continue;
    }

    widgets.column->setMinWidth(slotWidth);
    widgets.column->setSize(slotWidth, chartHeight);
    if (widgets.labelCell != nullptr) {
      widgets.labelCell->setMinWidth(slotWidth);
    }

    const float fillHeight =
        widgets.fillRatio > 0.0F ? std::min(chartHeight, std::max(2.0F * scale, chartHeight * widgets.fillRatio)) : 0.0F;
    const float barX = (slotWidth - barWidth) * 0.5F;

    if (widgets.fill != nullptr) {
      widgets.fill->setRadii(Radii{barRadius, barRadius, 0.0F, 0.0F});
      widgets.fill->setPosition(barX, chartHeight - fillHeight);
      widgets.fill->setSize(barWidth, fillHeight);
      widgets.fill->setVisible(fillHeight > 0.0F);
    }
  }

  m_chartPlotRow->layout(renderer);
  if (m_chartLabelRow != nullptr) {
    m_chartLabelRow->layout(renderer);
  }

  for (auto& widgets : m_chartColumns) {
    if (widgets.labelCell == nullptr || !widgets.labelCell->visible() || widgets.label == nullptr
        || !widgets.label->visible()) {
      continue;
    }
    widgets.label->measure(renderer);
    widgets.label->setPosition(
        (widgets.labelCell->width() - widgets.label->width()) * 0.5F,
        (widgets.labelCell->height() - widgets.label->height()) * 0.5F
    );
  }
}

void ScreenTimeTab::layoutAppRows(Renderer& renderer) {
  const float scale = contentScale();
  const float iconSize = kAppIconSize * scale;
  for (auto& widgets : m_appRows) {
    if (widgets.row == nullptr || !widgets.row->visible()) {
      continue;
    }
    if (widgets.iconSlot != nullptr) {
      widgets.iconSlot->setSize(iconSize, iconSize);
      if (widgets.icon != nullptr) {
        widgets.icon->setPosition(0.0F, 0.0F);
        widgets.icon->setSize(iconSize, iconSize);
      }
      if (widgets.iconFallback != nullptr) {
        widgets.iconFallback->measure(renderer);
        const float glyphSize = kAppIconSize * 0.55F * scale;
        widgets.iconFallback->setPosition((iconSize - glyphSize) * 0.5F, (iconSize - glyphSize) * 0.5F);
      }
    }
  }
}

void ScreenTimeTab::updateIconForRow(
    Renderer& renderer, AppRowWidgets& widgets, const std::string& appKey, float scale
) {
  const std::string iconPath = resolveIconPath(appKey);
  if (iconPath == widgets.iconPath) {
    return;
  }
  widgets.iconPath = iconPath;
  const int targetPx = static_cast<int>(std::round(kAppIconSize * scale));
  const bool hasIcon =
      !iconPath.empty() && widgets.icon != nullptr && widgets.icon->setSourceFile(renderer, iconPath, targetPx, true);
  if (widgets.icon != nullptr) {
    widgets.icon->setVisible(hasIcon);
  }
  if (widgets.iconFallback != nullptr) {
    widgets.iconFallback->setVisible(!hasIcon);
  }
}

std::string ScreenTimeTab::resolveIconPath(const std::string& appKey) const {
  if (appKey.empty() || appKey.starts_with("title:")) {
    return {};
  }

  std::string baseKey = appKey;
  if (const auto sep = baseKey.find('\x1F'); sep != std::string::npos) {
    baseKey = baseKey.substr(0, sep);
  }

  if (const auto internal = internal_apps::metadataForAppId(baseKey); internal.has_value()) {
    return internal->iconPath;
  }

  const int targetPx = static_cast<int>(std::round(kAppIconSize * contentScale()));
  std::string iconName;
  if (const auto entry = app_identity::findDesktopEntry(baseKey, desktopEntries());
      entry.has_value() && !entry->icon.empty()) {
    iconName = entry->icon;
  } else {
    iconName = app_identity::resolveRunningDesktopEntry(baseKey, desktopEntries()).icon;
  }
  if (!iconName.empty()) {
    const std::string& resolved = screenTimeIconResolver().resolve(iconName, targetPx);
    if (!resolved.empty()) {
      return resolved;
    }
  }
  const std::string& resolved = screenTimeIconResolver().resolve(baseKey, targetPx);
  return resolved.empty() ? std::string{} : std::string{resolved};
}

void ScreenTimeTab::onPanelCardOpacityChanged(float opacity) {
  if (m_rangePicker != nullptr) {
    m_rangePicker->setSurfaceOpacity(opacity);
  }
  if (m_statsCard != nullptr) {
    m_statsCard->setFill(colorSpecFromRole(ColorRole::SurfaceContainer, opacity));
  }
  if (m_appsCard != nullptr) {
    m_appsCard->setFill(colorSpecFromRole(ColorRole::SurfaceContainer, opacity));
  }
}
