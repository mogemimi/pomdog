// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/menu.h"
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

constexpr Key contextMenuKey = "devgui.menu.context"_key;
constexpr Key subMenuKey = "devgui.menu.sub"_key;

/// The time the pointer rests on an item before its submenu opens, or before
/// another item closes the open submenu. The pause lets the pointer cross
/// other items on its way to a submenu.
constexpr f64 subMenuDelay = 0.15;

/// The space between an item's label and its shortcut or submenu arrow.
constexpr f32 shortcutGap = 24.0f;

/// Describes the columns of one menu row.
struct MenuRow final {
    F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};
    f32 labelX = 0.0f;
    f32 rightX = 0.0f;
};

/// Allocates a menu row and asks the menu to fit labelWidth and rightWidth
/// in it.
[[nodiscard]] MenuRow
allocateRow(UI& ui, f32 labelWidth, f32 rightWidth)
{
    const auto& style = ui.getStyle();
    const auto checkColumn = style.fontSize + style.framePadding.x;
    auto width = checkColumn + labelWidth + style.framePadding.x;
    if (rightWidth > 0.0f) {
        width += shortcutGap + rightWidth;
    }

    MenuRow row;
    row.rect = ui.allocate(0.0f, style.getFrameHeight());
    ui.requestWidth(width);
    row.labelX = row.rect.x + checkColumn;
    row.rightX = row.rect.x + row.rect.width - style.framePadding.x;
    return row;
}

[[nodiscard]] Color
getItemTextColor(const Style& style, bool enabled, bool hot) noexcept
{
    if (!enabled) {
        auto color = style.textSecondaryColor;
        color.a = static_cast<u8>(color.a / 2);
        return color;
    }
    return hot ? style.textOnAccentColor : style.textColor;
}

void drawItemBackground(UI& ui, const F32Rect2D& rect, bool hot)
{
    if (hot) {
        const auto& style = ui.getStyle();
        ui.fillRect(rect, style.rowSelectedColor, style.frameRounding);
    }
}

} // namespace

MenuScope::MenuScope(UI& ui, WidgetID id, const F32Rect2D& anchor, PopupPlacement placement)
    : ui_(ui)
    , open_(ui.isPopupOpen(id))
{
    if (open_) {
        ui_.beginPopup(id, anchor, Vector2{0.0f, 0.0f}, placement);
    }
}

MenuScope::~MenuScope()
{
    if (open_) {
        ui_.endPopup();
    }
}

bool MenuScope::isOpen() const noexcept
{
    return open_;
}

WidgetID getContextMenuID(WidgetID id) noexcept
{
    return combineID(id, contextMenuKey);
}

MenuScope contextMenu(UI& ui, const Response& response)
{
    const auto id = getContextMenuID(response.id);
    if (response.secondaryClicked) {
        ui.openPopup(id);
    }
    const auto position = ui.getPopupOpenPosition(id);
    return MenuScope{ui, id, F32Rect2D{position.x, position.y, 0.0f, 0.0f}, PopupPlacement::Below};
}

MenuScope popupMenu(UI& ui, WidgetID id, const F32Rect2D& anchor)
{
    return MenuScope{ui, id, anchor, PopupPlacement::Below};
}

MenuScope subMenu(UI& ui, Key key, std::string_view label, bool enabled)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto chevronSize = std::round(style.fontSize * 0.22f);
    const auto row = allocateRow(ui, std::ceil(ui.measureText(label)), chevronSize);
    const auto response = ui.interact(id, row.rect);

    const auto popupID = combineID(id, subMenuKey);
    bool open = ui.isPopupOpen(popupID);
    if (enabled && response.hovered && !open) {
        const auto rested = ui.getHoveredDuration(id);
        if (response.active || (rested >= subMenuDelay)) {
            ui.openPopup(popupID);
            open = true;
        }
        else {
            ui.requestUpdateAt(ui.getInput().timeSeconds + (subMenuDelay - rested));
        }
    }

    // NOTE: The item stays highlighted while its submenu is open.
    const bool hot = enabled && (response.hovered || response.active || open);
    drawItemBackground(ui, row.rect, hot);
    const auto textColor = getItemTextColor(style, enabled, hot);
    const auto textTop = detail::getTextTop(style, row.rect);
    ui.drawText(Vector2{row.labelX, textTop}, label, textColor, std::max(0.0f, row.rightX - chevronSize - row.labelX));
    const auto chevronCenter = Vector2{row.rightX - chevronSize * 0.5f, row.rect.y + row.rect.height * 0.5f};
    detail::drawChevron(ui, chevronCenter, chevronSize, detail::ChevronDirection::Right, textColor);

    // NOTE: Line up the submenu's first item with this item, and its edge
    // with the edge of this menu.
    const auto& padding = style.popupPadding;
    const auto anchor = F32Rect2D{row.rect.x - padding.x, row.rect.y - padding.y, row.rect.width + padding.x * 2.0f, row.rect.height};
    return MenuScope{ui, popupID, anchor, PopupPlacement::Right};
}

Response menuItem(UI& ui, Key key, std::string_view label, const MenuItemOptions& options)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);
    const auto shortcutWidth = options.shortcut.empty() ? 0.0f : std::ceil(ui.measureText(options.shortcut));
    const auto row = allocateRow(ui, std::ceil(ui.measureText(label)), shortcutWidth);
    auto response = ui.interact(id, row.rect);
    if (!options.enabled) {
        response.clicked = false;
    }

    if (options.enabled && response.hovered && ui.hasChildPopups()) {
        // NOTE: Resting on this item closes the submenu that another item of
        // this menu opened.
        const auto rested = ui.getHoveredDuration(id);
        if (rested >= subMenuDelay) {
            ui.closeChildPopups();
        }
        else {
            ui.requestUpdateAt(ui.getInput().timeSeconds + (subMenuDelay - rested));
        }
    }

    const bool hot = options.enabled && (response.hovered || response.active);
    drawItemBackground(ui, row.rect, hot);
    const auto textColor = getItemTextColor(style, options.enabled, hot);
    if (options.checked) {
        const auto checkSize = style.fontSize * 0.8f;
        const auto checkBox = F32Rect2D{row.rect.x + style.framePadding.x * 0.5f, row.rect.y + (row.rect.height - checkSize) * 0.5f, checkSize, checkSize};
        detail::drawCheckMark(ui, checkBox, textColor);
    }

    const auto textTop = detail::getTextTop(style, row.rect);
    const auto labelRight = (shortcutWidth > 0.0f) ? (row.rightX - shortcutWidth - shortcutGap) : row.rightX;
    ui.drawText(Vector2{row.labelX, textTop}, label, textColor, std::max(0.0f, labelRight - row.labelX));
    if (shortcutWidth > 0.0f) {
        const auto shortcutColor = (hot || !options.enabled) ? textColor : style.textSecondaryColor;
        ui.drawText(Vector2{row.rightX - shortcutWidth, textTop}, options.shortcut, shortcutColor);
    }

    if (response.clicked) {
        ui.closePopup();
    }
    return response;
}

void menuSeparator(UI& ui)
{
    const auto& style = ui.getStyle();
    const auto rect = ui.allocate(0.0f, std::max(1.0f, style.itemSpacing.y));
    const auto y = std::floor(rect.y + rect.height * 0.5f);

    // NOTE: The line reaches the menu's edges.
    const auto& padding = style.popupPadding;
    ui.fillRect(F32Rect2D{rect.x - padding.x, y, rect.width + padding.x * 2.0f, 1.0f}, style.popupBorderColor);
}

} // namespace pomdog::devgui
