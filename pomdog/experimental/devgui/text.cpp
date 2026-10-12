// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/text.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

namespace pomdog::devgui {

void text(UI& ui, std::string_view text)
{
    devgui::text(ui, text, ui.getStyle().textColor);
}

void text(UI& ui, std::string_view text, const Color& color)
{
    const auto& style = ui.getStyle();

    // NOTE: A vertical layout gives the text the whole row, so only a
    // horizontal row needs the text width.
    auto width = 0.0f;
    if (ui.isLayoutHorizontal()) {
        width = ui.measureText(text);
    }
    const auto rect = ui.allocate(width, style.getFrameHeight());
    ui.drawText(Vector2{rect.x, detail::getTextTop(style, rect)}, text, color);
}

void labelText(UI& ui, std::string_view label, std::string_view text)
{
    const auto& style = ui.getStyle();
    const auto row = detail::layoutPropertyRow(ui, label);
    ui.drawText(Vector2{row.control.x, detail::getTextTop(style, row.control)}, text, style.textColor, row.control.width);
}

} // namespace pomdog::devgui
