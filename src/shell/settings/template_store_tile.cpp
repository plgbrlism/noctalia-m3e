#include "shell/settings/template_store_tile.h"

#include "i18n/i18n.h"
#include "render/scene/input_area.h"
#include "ui/builders.h"
#include "ui/controls/checkbox.h"
#include "ui/controls/label.h"
#include "ui/palette.h"
#include "ui/style.h"

namespace settings {

  TemplateStoreTile::TemplateStoreTile(float scale) : m_scale(scale) {
    const float actionSize = Style::checkboxTargetSize * scale;

    setDirection(FlexDirection::Horizontal);
    setAlign(FlexAlign::Center);
    setGap(Style::spaceXs * scale);
    setPadding(Style::spaceXs * scale, Style::spaceSm * scale);
    setMinHeight((Style::checkboxTargetSize + Style::spaceXs * 2.0F) * scale);
    setFill(colorSpecFromRole(ColorRole::SurfaceContainerLow));
    setBorder(colorSpecFromRole(ColorRole::OutlineVariant, Style::disabledOutlineAlpha), Style::borderWidth * scale);
    setRadius(Style::scaledRadiusMd(scale));

    auto text = ui::column({.align = FlexAlign::Start, .justify = FlexJustify::Center, .flexGrow = 1.0F});
    text->setHitTestVisible(false);
    text->addChild(
        ui::label({
            .out = &m_nameLabel,
            .fontSize = Style::fontSizeBody * scale,
            .fontWeight = FontWeight::Medium,
            .color = colorSpecFromRole(ColorRole::OnSurface),
            .maxLines = 1,
            .ellipsize = TextEllipsize::End,
        })
    );
    text->addChild(
        ui::label({
            .out = &m_categoryLabel,
            .fontSize = Style::fontSizeCaption * scale,
            .color = colorSpecFromRole(ColorRole::OnSurfaceVariant),
            .maxLines = 1,
            .ellipsize = TextEllipsize::End,
            .visible = false,
        })
    );
    addChild(std::move(text));

    auto actionGroup = ui::row({.align = FlexAlign::Center, .gap = Style::spaceSm * scale});
    actionGroup->setZIndex(10);
    actionGroup->addChild(
        ui::button({
            .out = &m_readmeButton,
            .glyph = "external-link",
            .glyphSize = 20.0F * scale,
            .variant = ButtonVariant::Ghost,
            .tooltip = i18n::tr("settings.templates.documentation"),
            .minWidth = 40.0F * scale,
            .minHeight = 40.0F * scale,
            .width = 40.0F * scale,
            .height = 40.0F * scale,
            .visible = false,
            .participatesInLayout = false,
            .configure = [scale](Button& button) {
              button.setPadding(0.0F);
              button.setRadius(20.0F * scale);
              button.setZIndex(20);
              auto palette = Button::defaultPalette(ButtonVariant::Ghost);
              palette.hover.bg = colorSpecFromRole(ColorRole::SurfaceContainerHigh);
              button.setCustomPalette(palette);
            },
        })
    );

    auto checkboxHost = ui::button({
        .variant = ButtonVariant::Ghost,
        .minWidth = actionSize,
        .minHeight = actionSize,
        .width = actionSize,
        .height = actionSize,
        .configure = [scale](Button& button) {
          button.setPadding(0.0F);
          button.setRadius(Style::scaledRadiusLg(scale));
          button.setZIndex(20);
          if (auto* area = button.inputArea()) {
            area->setHitTestVisible(false);
            area->setAcceptedButtons(0);
          }
        },
    });
    checkboxHost->addChild(
        ui::checkbox({
            .out = &m_checkbox,
            .scale = scale,
            .checkedFill = colorSpecFromRole(ColorRole::Primary),
            .checkedGlyph = colorSpecFromRole(ColorRole::OnPrimary),
        })
    );
    actionGroup->addChild(std::move(checkboxHost));
    addChild(std::move(actionGroup));

    auto tooltipArea = std::make_unique<InputArea>();
    tooltipArea->setParticipatesInLayout(false);
    tooltipArea->setHitTestVisible(true);
    tooltipArea->setFrameSize(0.0F, 0.0F);
    tooltipArea->setZIndex(1);
    m_tooltipArea = static_cast<InputArea*>(addChild(std::move(tooltipArea)));
  }

  void TemplateStoreTile::doLayout(Renderer& renderer) {
    Flex::doLayout(renderer);
    if (m_tooltipArea != nullptr) {
      m_tooltipArea->setPosition(0.0F, 0.0F);
      m_tooltipArea->setFrameSize(width(), height());
    }
  }

  void TemplateStoreTile::bind(
      std::string_view name, std::string_view category, bool enabled, bool hasReadme,
      std::string_view tooltip, std::function<void(bool enabled)> onToggle, std::function<void()> onReadme
  ) {
    m_nameLabel->setText(name);
    m_categoryLabel->setText(category);

    const bool hasCategory = !category.empty();
    m_categoryLabel->setVisible(hasCategory);
    m_categoryLabel->setParticipatesInLayout(hasCategory);
    m_readmeButton->setVisible(hasReadme);
    m_readmeButton->setParticipatesInLayout(hasReadme);
    if (m_tooltipArea != nullptr) {
      if (tooltip.empty()) {
        m_tooltipArea->clearTooltip();
      } else {
        m_tooltipArea->setTooltip(std::string(tooltip));
      }
    }
    if (m_checkbox != nullptr) {
      if (tooltip.empty()) {
        m_checkbox->clearTooltip();
      } else {
        m_checkbox->setTooltip(tooltip);
      }
    }
    m_readmeButton->setOnClick([onReadme = std::move(onReadme)] {
      if (onReadme) {
        onReadme();
      }
    });
    auto readmePalette = Button::defaultPalette(ButtonVariant::Ghost);
    readmePalette.normal.label = colorSpecFromRole(ColorRole::OnSurface);
    readmePalette.hover.bg = colorSpecFromRole(ColorRole::Surface);
    readmePalette.hover.label = colorSpecFromRole(ColorRole::OnSurface);
    m_readmeButton->setCustomPalette(readmePalette);

    if (m_checkbox != nullptr) {
      m_checkbox->setOnChange(nullptr);
      m_checkbox->setChecked(enabled);
      m_checkbox->setOnChange([onToggle = std::move(onToggle)](bool next) {
        if (onToggle) {
          onToggle(next);
        }
      });
    }

    const float borderWidth = Style::borderWidth * m_scale;
    setFill(colorSpecFromRole(ColorRole::SurfaceContainerLow));
    setBorder(colorSpecFromRole(ColorRole::OutlineVariant, Style::disabledOutlineAlpha), borderWidth);
    m_nameLabel->setColor(colorSpecFromRole(ColorRole::OnSurface));
    m_categoryLabel->setColor(colorSpecFromRole(ColorRole::OnSurfaceVariant));
  }

} // namespace settings
