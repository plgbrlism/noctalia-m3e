#pragma once

#include "ui/controls/flex.h"

#include <functional>
#include <string>
#include <string_view>

class Button;
class Checkbox;
class InputArea;
class Label;

namespace settings {

  class TemplateStoreTile : public Flex {
  public:
    explicit TemplateStoreTile(float scale);

    void bind(
        std::string_view name, std::string_view category, bool enabled, bool hasReadme,
        std::string_view tooltip, std::function<void(bool enabled)> onToggle, std::function<void()> onReadme
    );

  protected:
    void doLayout(Renderer& renderer) override;

  private:
    float m_scale;
    Checkbox* m_checkbox = nullptr;
    Label* m_nameLabel = nullptr;
    Label* m_categoryLabel = nullptr;
    Button* m_readmeButton = nullptr;
    InputArea* m_tooltipArea = nullptr;
  };

} // namespace settings
