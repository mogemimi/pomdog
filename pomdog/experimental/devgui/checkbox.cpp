// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/checkbox.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

Response checkbox(UI& ui, Key key, std::string_view label, bool& value)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);

    auto response = ui.interact(id, row.row);
    if (response.clicked) {
        value = !value;
        response.changed = true;
    }

    const auto boxSize = style.fontSize;
    const auto box = F32Rect2D{
        row.control.x,
        std::round(row.control.y + (row.control.height - boxSize) * 0.5f),
        boxSize,
        boxSize,
    };

    if (value) {
        const auto fill = response.hovered ? style.accentHoveredColor : style.accentColor;
        ui.fillRect(box, fill, style.frameRounding);
        detail::drawCheckMark(ui, box, style.textOnAccentColor);
    }
    else {
        const auto border = detail::getInputBorderColor(style, response, false);
        detail::drawFrame(ui, box, style.inputBackgroundColor, border, style.frameRounding);
    }
    return response;
}

} // namespace pomdog::devgui
