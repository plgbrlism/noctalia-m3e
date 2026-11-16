#include "shell/settings/template_store_content.h"

#include "config/config_service.h"
#include "i18n/i18n.h"
#include "shell/settings/template_store_tile.h"
#include "ui/builders.h"
#include "ui/controls/button.h"
#include "ui/controls/input.h"
#include "ui/controls/label.h"
#include "ui/controls/scroll_view.h"
#include "ui/controls/segmented.h"
#include "ui/palette.h"
#include "ui/style.h"
#include "util/string_utils.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <numeric>
#include <set>
#include <unordered_map>

namespace settings {

  namespace {

    // Sentinel for the Enabled chip — not a real catalog category value.
    constexpr std::string_view kEnabledFilter = "__enabled__";

    bool containsInsensitive(std::string_view haystack, std::string_view needle) {
      if (needle.empty()) {
        return true;
      }
      return !std::ranges::search(haystack, needle, [](char a, char b) {
                return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
              }).empty();
    }

  } // namespace

  TemplateStoreContent::TemplateStoreContent(
      std::vector<noctalia::theme::AvailableTemplate> catalog, std::unordered_set<std::string> selectedIds,
      ConfigService* config, TemplateStoreCallbacks callbacks
  )
      : m_catalog(std::move(catalog)), m_selectedIds(std::move(selectedIds)), m_config(config),
        m_callbacks(std::move(callbacks)) {
    if (m_config != nullptr) {
      if (const std::optional<std::string> sort = m_config->stateString("template_store", "sort")) {
        m_sortMode = sortModeFromState(*sort);
      }
    }
    collectCategories();
    sortFiltered();
    applyFilter();
  }

  TemplateStoreContent::~TemplateStoreContent() = default;

  void TemplateStoreContent::collectCategories() {
    std::set<std::string> categories;
    for (const auto& entry : m_catalog) {
      if (!entry.category.empty()) {
        categories.insert(entry.category);
      }
    }
    m_allCategories.assign(categories.begin(), categories.end());
  }

  void TemplateStoreContent::sortFiltered() {
    m_sortedIndices.resize(m_catalog.size());
    std::iota(m_sortedIndices.begin(), m_sortedIndices.end(), std::size_t{0});
    std::ranges::sort(m_sortedIndices, [this](std::size_t a, std::size_t b) {
      const auto& left = m_catalog[a];
      const auto& right = m_catalog[b];
      const int nameOrder = StringUtils::naturalCaseInsensitiveCompare(left.displayName, right.displayName);
      const int categoryOrder = StringUtils::naturalCaseInsensitiveCompare(left.category, right.category);
      switch (m_sortMode) {
      case TemplateSortMode::NameDesc:
        return nameOrder > 0;
      case TemplateSortMode::CategoryAsc:
        return categoryOrder != 0 ? categoryOrder < 0 : nameOrder < 0;
      case TemplateSortMode::CategoryDesc:
        return categoryOrder != 0 ? categoryOrder > 0 : nameOrder < 0;
      case TemplateSortMode::NameAsc:
      default:
        return nameOrder < 0;
      }
    });
  }

  void TemplateStoreContent::cycleSortMode() {
    TemplateSortMode next = TemplateSortMode::NameAsc;
    switch (m_sortMode) {
    case TemplateSortMode::NameAsc:
      next = TemplateSortMode::NameDesc;
      break;
    case TemplateSortMode::NameDesc:
      next = TemplateSortMode::CategoryAsc;
      break;
    case TemplateSortMode::CategoryAsc:
      next = TemplateSortMode::CategoryDesc;
      break;
    case TemplateSortMode::CategoryDesc:
      next = TemplateSortMode::NameAsc;
      break;
    }
    setSortMode(next);
  }

  void TemplateStoreContent::setSortMode(TemplateSortMode mode) {
    if (m_sortMode == mode) {
      return;
    }
    m_sortMode = mode;
    if (m_config != nullptr) {
      m_config->setStateString("template_store", "sort", sortModeStateValue(mode));
    }
    syncSortButtonGlyph();

    if (m_listContent == nullptr || m_rows.empty()) {
      sortFiltered();
      applyFilter();
      return;
    }

    const std::vector<std::size_t> oldSorted = m_sortedIndices;
    std::unordered_map<std::size_t, std::unique_ptr<Node>> owned;
    owned.reserve(m_rows.size());
    for (std::size_t i = 0; i < m_rows.size() && i < oldSorted.size(); ++i) {
      if (m_rows[i] != nullptr) {
        owned[oldSorted[i]] = m_listContent->removeChild(static_cast<Node*>(m_rows[i]));
      }
    }

    sortFiltered();
    m_rows.clear();
    m_rows.reserve(m_sortedIndices.size());
    for (const std::size_t catalogIndex : m_sortedIndices) {
      auto it = owned.find(catalogIndex);
      if (it == owned.end()) {
        continue;
      }
      m_rows.push_back(static_cast<TemplateStoreTile*>(it->second.get()));
      m_listContent->addChild(std::move(it->second));
    }
    applyFilter();
  }

  TemplateSortMode TemplateStoreContent::sortModeFromState(std::string_view value) {
    if (value == "name_desc") {
      return TemplateSortMode::NameDesc;
    }
    if (value == "category_asc") {
      return TemplateSortMode::CategoryAsc;
    }
    if (value == "category_desc") {
      return TemplateSortMode::CategoryDesc;
    }
    return TemplateSortMode::NameAsc;
  }

  std::string_view TemplateStoreContent::sortModeStateValue(TemplateSortMode mode) {
    switch (mode) {
    case TemplateSortMode::NameDesc:
      return "name_desc";
    case TemplateSortMode::CategoryAsc:
      return "category_asc";
    case TemplateSortMode::CategoryDesc:
      return "category_desc";
    case TemplateSortMode::NameAsc:
    default:
      return "name_asc";
    }
  }

  std::string_view TemplateStoreContent::sortModeGlyph(TemplateSortMode mode) {
    switch (mode) {
    case TemplateSortMode::NameDesc:
      return "sort-z-a";
    case TemplateSortMode::CategoryAsc:
      return "sort-ascending-2";
    case TemplateSortMode::CategoryDesc:
      return "sort-descending-2";
    case TemplateSortMode::NameAsc:
    default:
      return "sort-a-z";
    }
  }

  const char* TemplateStoreContent::sortModeTooltipKey(TemplateSortMode mode) {
    switch (mode) {
    case TemplateSortMode::NameDesc:
      return "settings.templates.store.sort-name-desc";
    case TemplateSortMode::CategoryAsc:
      return "settings.templates.store.sort-category-asc";
    case TemplateSortMode::CategoryDesc:
      return "settings.templates.store.sort-category-desc";
    case TemplateSortMode::NameAsc:
    default:
      return "settings.templates.store.sort-name-asc";
    }
  }

  void TemplateStoreContent::syncSortButtonGlyph() {
    if (m_sortButton == nullptr) {
      return;
    }
    m_sortButton->setGlyph(sortModeGlyph(m_sortMode));
    m_sortButton->setTooltip(i18n::tr(sortModeTooltipKey(m_sortMode)));
  }

  void TemplateStoreContent::applyFilter() {
    m_visible.assign(m_rows.size(), false);
    m_filteredIndices.clear();
    std::size_t visibleCount = 0;

    const bool enabledOnly = m_selectedCategory == kEnabledFilter;
    for (std::size_t i = 0; i < m_rows.size(); ++i) {
      const std::size_t catalogIndex = m_sortedIndices[i];
      const auto& entry = m_catalog[catalogIndex];

      bool visible = true;
      if (enabledOnly) {
        visible = m_selectedIds.contains(entry.id);
      } else if (!m_selectedCategory.empty()) {
        visible = entry.category == m_selectedCategory;
      }
      if (visible && !m_searchQuery.empty()) {
        visible = containsInsensitive(entry.displayName, m_searchQuery)
            || containsInsensitive(entry.id, m_searchQuery)
            || containsInsensitive(entry.category, m_searchQuery);
      }

      m_visible[i] = visible;
      if (m_rows[i] != nullptr) {
        m_rows[i]->setVisible(visible);
        m_rows[i]->setParticipatesInLayout(visible);
      }
      if (visible) {
        ++visibleCount;
        m_filteredIndices.push_back(catalogIndex);
      }
    }

    if (m_countLabel != nullptr) {
      m_countLabel->setText(
          i18n::tr("settings.templates.store.results-count", "count", std::to_string(visibleCount))
      );
    }

    if (m_emptyLabel != nullptr) {
      m_emptyLabel->setText(
          m_catalog.empty() ? i18n::tr("settings.schema.templates.community-ids.empty")
                            : i18n::tr("settings.templates.store.empty")
      );
      m_emptyLabel->setVisible(visibleCount == 0);
      m_emptyLabel->setParticipatesInLayout(visibleCount == 0);
    }
  }

  void TemplateStoreContent::commitSelection() {
    if (!m_callbacks.setSelected) {
      return;
    }
    std::vector<std::string> ordered;
    ordered.reserve(m_selectedIds.size());
    for (const auto& entry : m_catalog) {
      if (m_selectedIds.contains(entry.id)) {
        ordered.push_back(entry.id);
      }
    }
    m_callbacks.setSelected(std::move(ordered));
  }

  void TemplateStoreContent::populateBody(Flex& body, Renderer* /*renderer*/) {
    const float scale = m_callbacks.scale;
    m_countLabel = nullptr;
    m_sortButton = nullptr;
    m_searchInput = nullptr;
    m_categoryFilter = nullptr;
    m_listScroll = nullptr;
    m_listContent = nullptr;
    m_emptyLabel = nullptr;
    m_rows.clear();

    body.setDirection(FlexDirection::Vertical);
    body.setAlign(FlexAlign::Stretch);
    body.setGap(Style::spaceSm * scale);
    body.setPadding(Style::spaceMd * scale);

    body.addChild(
        ui::input({
            .out = &m_searchInput,
            .value = m_searchQuery,
            .placeholder = i18n::tr("settings.templates.store.search-placeholder"),
            .fontSize = Style::fontSizeBody * scale,
            .controlHeight = Style::controlHeight * scale,
            .horizontalPadding = Style::spaceMd * scale,
            .clearButtonEnabled = true,
            .onChange =
                [this](const std::string& text) {
                  m_searchQuery = text;
                  applyFilter();
                },
        })
    );

    auto categoryScroll = ui::scrollView({
        .orientation = ScrollOrientation::Horizontal,
        .scrollbarVisible = false,
        .viewportPaddingH = 0.0F,
        .viewportPaddingV = 0.0F,
        .fill = clearColorSpec(),
        .radius = 0.0F,
        .fillWidth = true,
        .configure = [](ScrollView& scroll) {
          scroll.clearFill();
          scroll.clearBorder();
        },
    });
    categoryScroll->content()->setAlign(FlexAlign::Center);
    auto categoryFilter = ui::segmented({
        .out = &m_categoryFilter,
        .scale = scale,
        .compact = true,
        .surfaceOpacity = 0.0F,
        .equalSegmentWidths = false,
        .configure = [](Segmented& segmented) { segmented.setAlign(FlexAlign::Center); },
    });

    std::vector<std::string> filterKeys;
    filterKeys.reserve(m_allCategories.size() + 2);
    filterKeys.emplace_back();
    filterKeys.emplace_back(kEnabledFilter);
    for (const auto& category : m_allCategories) {
      filterKeys.push_back(category);
    }

    std::size_t selectedIndex = 0;
    for (std::size_t i = 0; i < filterKeys.size(); ++i) {
      if (filterKeys[i] == m_selectedCategory) {
        selectedIndex = i;
        break;
      }
    }

    categoryFilter->addOption(i18n::tr("settings.templates.store.category-all"));
    categoryFilter->addOption(i18n::tr("settings.templates.store.filter-enabled"));
    for (const auto& category : m_allCategories) {
      categoryFilter->addOption(category);
    }
    categoryFilter->setSelectedIndex(selectedIndex);
    categoryFilter->setOnChange(
        [this, filterKeys = std::move(filterKeys)](std::size_t index) {
          if (index >= filterKeys.size()) {
            return;
          }
          m_selectedCategory = filterKeys[index];
          applyFilter();
        }
    );
    categoryScroll->content()->addChild(std::move(categoryFilter));
    body.addChild(std::move(categoryScroll));

    auto summaryRow = ui::row({
        .align = FlexAlign::Center,
        .gap = Style::spaceSm * scale,
        .fillWidth = true,
    });
    summaryRow->addChild(
        ui::label({
            .out = &m_countLabel,
            .fontSize = Style::fontSizeCaption * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
        })
    );
    summaryRow->addChild(ui::spacer());
    summaryRow->addChild(
        ui::button({
            .out = &m_sortButton,
            .glyph = std::string(sortModeGlyph(m_sortMode)),
            .glyphSize = Style::fontSizeTitle * scale,
            .variant = ButtonVariant::Default,
            .tooltip = i18n::tr(sortModeTooltipKey(m_sortMode)),
            .minWidth = Style::controlHeight * scale,
            .minHeight = Style::controlHeight * scale,
            .padding = Style::spaceXs * scale,
            .radius = Style::scaledRadiusMd(scale),
            .onClick = [this] { cycleSortMode(); },
        })
    );
    body.addChild(std::move(summaryRow));

    auto listScroll = ui::scrollView({
        .out = &m_listScroll,
        .contentScale = scale,
        .scrollbarVisible = true,
        .viewportPaddingH = 0.0F,
        .viewportPaddingV = 0.0F,
        .fill = clearColorSpec(),
        .radius = 0.0F,
        .fillWidth = true,
        .flexGrow = 1.0F,
        .configure = [](ScrollView& scroll) {
          scroll.clearFill();
          scroll.clearBorder();
        },
    });
    m_listContent = listScroll->content();
    m_listContent->setDirection(FlexDirection::Vertical);
    m_listContent->setAlign(FlexAlign::Stretch);
    m_listContent->setGap(Style::spaceXs * scale);
    m_listContent->addChild(ui::box({.fill = clearColorSpec(), .height = Style::spaceMd * scale}));
    body.addChild(std::move(listScroll));

    body.addChild(
        ui::label({
            .out = &m_emptyLabel,
            .fontSize = Style::fontSizeCaption * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
            .visible = false,
            .participatesInLayout = false,
        })
    );

    sortFiltered();
    m_rows.reserve(m_sortedIndices.size());
    auto refreshCount = [this] {
      std::size_t visibleCount = 0;
      for (const bool visible : m_visible) {
        if (visible) {
          ++visibleCount;
        }
      }
      if (m_countLabel != nullptr) {
        m_countLabel->setText(
            i18n::tr("settings.templates.store.results-count", "count", std::to_string(visibleCount))
        );
      }
    };

    for (const std::size_t catalogIndex : m_sortedIndices) {
      const auto& entry = m_catalog[catalogIndex];
      auto tile = std::make_unique<TemplateStoreTile>(scale);
      auto* row = tile.get();
      m_listContent->addChild(std::move(tile));
      m_rows.push_back(row);

      const std::string id = entry.id;
      row->bind(
          entry.displayName, entry.category, m_selectedIds.contains(entry.id), !entry.documentationUrl.empty(),
          noctalia::theme::formatTemplateTooltip(entry),
          [this, id, refreshCount](bool next) {
            if (next) {
              m_selectedIds.insert(id);
            } else {
              m_selectedIds.erase(id);
            }
            commitSelection();
            refreshCount();
          },
          [this, catalogIndex] {
            for (std::size_t fi = 0; fi < m_filteredIndices.size(); ++fi) {
              if (m_filteredIndices[fi] == catalogIndex) {
                (void)openDocumentationAtFilteredIndex(fi);
                break;
              }
            }
          }
      );
    }

    m_listContent->addChild(ui::box({.fill = clearColorSpec(), .height = Style::spaceMd * scale}));

    applyFilter();
  }

  bool TemplateStoreContent::openDocumentationAtFilteredIndex(std::size_t filteredIndex) {
    if (filteredIndex >= m_filteredIndices.size() || !m_callbacks.openBrowserUrl) {
      return false;
    }
    const auto& url = m_catalog[m_filteredIndices[filteredIndex]].documentationUrl;
    if (url.empty()) {
      return false;
    }
    m_callbacks.openBrowserUrl(url);
    return true;
  }

} // namespace settings
