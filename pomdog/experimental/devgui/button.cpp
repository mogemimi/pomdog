// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/button.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

Response button(UI& ui, Key key, std::string_view label)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto width = std::ceil(ui.measureText(label)) + style.framePadding.x * 2.0f;
    const auto rect = ui.allocate(width, style.getFrameHeight());
    const auto response = ui.interact(id, rect);

    auto color = style.accentColor;
    if (response.active && response.hovered) {
        color = style.accentActiveColor;
    }
    else if (response.hovered) {
        color = style.accentHoveredColor;
    }
    ui.fillRect(rect, color, style.frameRounding);
    ui.drawText(Vector2{rect.x + style.framePadding.x, detail::getTextTop(style, rect)}, label, style.textOnAccentColor);
    return response;
}

Response closeButton(UI& ui, Key key, const F32Rect2D& rect)
{
    const auto response = ui.interact(ui.makeID(key), rect);
    detail::drawCloseButton(ui, rect, response, true);
    return response;
}

} // namespace pomdog::devgui
