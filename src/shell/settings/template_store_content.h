#pragma once

#include "theme/builtin_templates.h"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

class Button;
class ConfigService;
class Flex;
class Input;
class Label;
class Renderer;
class ScrollView;
class Segmented;

namespace settings {

  class TemplateStoreTile;

  enum class TemplateSortMode : std::uint8_t {
    NameAsc,
    NameDesc,
    CategoryAsc,
    CategoryDesc,
  };

  struct TemplateStoreCallbacks {
    // Called with catalog-ordered enabled ids whenever the selection changes.
    std::function<void(std::vector<std::string> ids)> setSelected;
    std::function<void(const std::string&)> openBrowserUrl;
    float scale = 1.0F;
  };

  class TemplateStoreContent {
  public:
    TemplateStoreContent(
        std::vector<vespertine::theme::AvailableTemplate> catalog, std::unordered_set<std::string> selectedIds,
        ConfigService* config, TemplateStoreCallbacks callbacks
    );
    ~TemplateStoreContent();

    void populateBody(Flex& body, Renderer* renderer = nullptr);

    [[nodiscard]] bool openDocumentationAtFilteredIndex(std::size_t filteredIndex);

  private:
    void collectCategories();
    void applyFilter();
    void sortFiltered();
    void cycleSortMode();
    void setSortMode(TemplateSortMode mode);
    void syncSortButtonGlyph();
    static TemplateSortMode sortModeFromState(std::string_view value);
    static std::string_view sortModeStateValue(TemplateSortMode mode);
    static std::string_view sortModeGlyph(TemplateSortMode mode);
    static const char* sortModeTooltipKey(TemplateSortMode mode);
    void commitSelection();

    std::vector<vespertine::theme::AvailableTemplate> m_catalog;
    std::unordered_set<std::string> m_selectedIds;
    std::vector<std::size_t> m_filteredIndices;
    std::vector<std::size_t> m_sortedIndices;
    std::vector<bool> m_visible;
    std::vector<TemplateStoreTile*> m_rows;
    std::vector<std::string> m_allCategories;
    TemplateSortMode m_sortMode = TemplateSortMode::NameAsc;
    std::string m_searchQuery;
    // Empty = all templates. `kEnabledFilter` shows only enabled. Otherwise a catalog category.
    std::string m_selectedCategory;
    ConfigService* m_config = nullptr;
    TemplateStoreCallbacks m_callbacks;

    Label* m_countLabel = nullptr;
    Button* m_sortButton = nullptr;
    Input* m_searchInput = nullptr;
    Segmented* m_categoryFilter = nullptr;
    ScrollView* m_listScroll = nullptr;
    Flex* m_listContent = nullptr;
    Label* m_emptyLabel = nullptr;
  };

} // namespace settings
