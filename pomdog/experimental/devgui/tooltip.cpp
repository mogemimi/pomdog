// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/tooltip.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

/// The offset of a tooltip from the pointer, clear of a typical cursor.
constexpr f32 pointerOffsetX = 12.0f;
constexpr f32 pointerOffsetY = 20.0f;

/// The space kept between a tooltip and the viewport edges.
constexpr f32 viewportMargin = 4.0f;

/// Calls func with each line of text, which is separated by '\n'.
template <typename Func>
void forEachLine(std::string_view text, Func&& func)
{
    while (true) {
        const auto end = text.find('\n');
        func(text.substr(0, end));
        if (end == std::string_view::npos) {
            break;
        }
        text.remove_prefix(end + 1);
    }
}

} // namespace

void tooltip(UI& ui, const Response& response, std::string_view text, i18n::LineBreakLanguage language)
{
    if (!response.hovered || response.active || text.empty()) {
        return;
    }

    const auto& style = ui.getStyle();
    const auto& input = ui.getInput();
    const auto hoveredDuration = ui.getHoveredDuration(response.id);
    if (hoveredDuration < style.tooltipDelay) {
        ui.requestUpdateAt(input.timeSeconds + (style.tooltipDelay - hoveredDuration));
        return;
    }

    const auto wrapped = ui.wrapText(response.id, text, style.tooltipMaxWidth, language);

    auto textWidth = 0.0f;
    auto lineCount = 0;
    forEachLine(wrapped, [&](std::string_view line) {
        textWidth = std::max(textWidth, ui.measureText(line));
        ++lineCount;
    });

    const auto lineHeight = std::round(style.getLineHeight());
    const auto& padding = style.popupPadding;
    const auto width = std::ceil(textWidth) + padding.x * 2.0f;
    const auto height = lineHeight * static_cast<f32>(lineCount - 1) + style.fontSize + padding.y * 2.0f;

    // NOTE: Place the tooltip below and to the right of the pointer, and flip
    // it to the other side of the pointer when it would leave the viewport.
    auto x = input.pointerPosition.x + pointerOffsetX;
    auto y = input.pointerPosition.y + pointerOffsetY;
    if (x + width > input.viewportSize.x - viewportMargin) {
        x = input.viewportSize.x - viewportMargin - width;
    }
    if (y + height > input.viewportSize.y - viewportMargin) {
        y = input.pointerPosition.y - viewportMargin - height;
    }
    const auto rect = F32Rect2D{std::round(std::max(viewportMargin, x)), std::round(std::max(viewportMargin, y)), width, height};

    ui.beginOverlay();

    auto shadow = rect;
    shadow.y += 2.0f;
    ui.fillRect(shadow, style.shadowColor, style.popupRounding);
    detail::drawFrame(ui, rect, style.tooltipBackgroundColor, style.popupBorderColor, style.popupRounding);

    auto lineTop = rect.y + padding.y;
    forEachLine(wrapped, [&](std::string_view line) {
        ui.drawText(Vector2{rect.x + padding.x, lineTop}, line, style.textColor);
        lineTop += lineHeight;
    });

    ui.endOverlay();
}

} // namespace pomdog::devgui
