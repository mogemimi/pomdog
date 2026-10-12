// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/combo.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

using namespace pomdog::devgui::literals;

constexpr Key popupKey = "devgui.combo.popup"_key;
constexpr Key itemKey = "devgui.combo.item"_key;

/// The most items the list shows before it scrolls.
constexpr i32 maxVisibleItems = 10;

} // namespace

Response combo(UI& ui, Key key, std::string_view label, i32& selectedIndex, std::span<const std::string_view> items)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto row = detail::layoutPropertyRow(ui, label);
    const auto box = row.control;
    auto response = ui.interact(id, box);

    const auto popupID = combineID(id, popupKey);
    bool open = ui.isPopupOpen(popupID);
    if (response.clicked && !open) {
        ui.openPopup(popupID);
        open = true;
    }

    const auto itemCount = static_cast<i32>(items.size());
    const bool hasSelection = (selectedIndex >= 0) && (selectedIndex < itemCount);

    const auto border = detail::getInputBorderColor(style, response, open);
    detail::drawFrame(ui, box, style.inputBackgroundColor, border, style.frameRounding);

    const auto chevronWidth = box.height;
    if (hasSelection) {
        const auto textWidth = box.width - style.framePadding.x - chevronWidth;
        const auto text = items[static_cast<std::size_t>(selectedIndex)];
        ui.drawText(Vector2{box.x + style.framePadding.x, detail::getTextTop(style, box)}, text, style.textColor, textWidth);
    }
    const auto chevronCenter = Vector2{box.x + box.width - chevronWidth * 0.5f, box.y + box.height * 0.5f};
    detail::drawChevron(ui, chevronCenter, std::round(style.fontSize * 0.22f), detail::ChevronDirection::Down, style.textSecondaryColor);

    if (!open) {
        return response;
    }

    const auto rowHeight = style.getFrameHeight();
    const auto visibleItems = std::clamp(itemCount, 1, maxVisibleItems);
    const auto listHeight = rowHeight * static_cast<f32>(visibleItems) +
                            style.itemSpacing.y * static_cast<f32>(visibleItems - 1) +
                            style.popupPadding.y * 2.0f;

    ui.beginPopup(popupID, box, Vector2{box.width, listHeight});
    const auto checkWidth = style.fontSize + style.framePadding.x;
    for (i32 i = 0; i < itemCount; ++i) {
        const auto rect = ui.allocate(0.0f, rowHeight);
        const auto itemResponse = ui.interact(ui.makeID(itemKey.at(static_cast<u32>(i))), rect);

        const bool hot = itemResponse.hovered || itemResponse.active;
        if (hot) {
            ui.fillRect(rect, style.rowSelectedColor, style.frameRounding);
        }
        if (i == selectedIndex) {
            const auto checkSize = style.fontSize * 0.8f;
            const auto checkBox = F32Rect2D{rect.x + style.framePadding.x * 0.5f, rect.y + (rect.height - checkSize) * 0.5f, checkSize, checkSize};
            detail::drawCheckMark(ui, checkBox, style.textColor);
        }

        const auto textX = rect.x + checkWidth;
        const auto text = items[static_cast<std::size_t>(i)];
        ui.drawText(Vector2{textX, detail::getTextTop(style, rect)}, text, hot ? style.textOnAccentColor : style.textColor, rect.x + rect.width - textX);

        if (itemResponse.clicked) {
            response.changed = (selectedIndex != i);
            selectedIndex = i;
            ui.closePopup();

            // NOTE: The box was drawn before the choice. Ask for another frame
            // so that it shows the new item even if no input follows.
            ui.requestUpdateAt(ui.getInput().timeSeconds);
        }
    }
    ui.endPopup();
    return response;
}

} // namespace pomdog::devgui
