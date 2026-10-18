// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
enum class PopupPlacement : u8;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Describes a menu item beyond its label.
struct MenuItemOptions final {
    /// The keyboard shortcut shown at the right of the item, such as "Ctrl+C".
    /// Showing it does not bind the keys; the application handles them.
    std::string_view shortcut = {};

    /// True to show a check mark before the label.
    bool checked = false;

    /// False to gray the item out and ignore clicks on it.
    bool enabled = true;
};

/// Keeps a menu open until destroyed. Create one with `devgui::contextMenu()`,
/// `devgui::popupMenu()`, or `devgui::subMenu()`.
class [[nodiscard]] MenuScope final {
public:
    /// Starts the menu id next to anchor if it is open.
    MenuScope(UI& ui, WidgetID id, const F32Rect2D& anchor, PopupPlacement placement);
    ~MenuScope();

    MenuScope(const MenuScope&) = delete;
    MenuScope& operator=(const MenuScope&) = delete;
    MenuScope(MenuScope&&) = delete;
    MenuScope& operator=(MenuScope&&) = delete;

    /// Returns true when the menu is open. Build its items only then.
    [[nodiscard]] bool
    isOpen() const noexcept;

private:
    UI& ui_;
    bool open_ = false;
};

/// Opens a context menu for the widget that response describes when the user
/// secondary-clicks it, usually with the right mouse button, and builds the
/// menu while it is open.
///
/// The menu opens where the pointer was released and sizes itself to its
/// items. Call it right after the widget, in the same ID scope:
/// ```cpp
/// const auto row = devgui::treeNode(ui, "emitter"_key.at(id), name, ...);
/// if (auto menu = devgui::contextMenu(ui, row); menu.isOpen()) {
///     if (devgui::menuItem(ui, "duplicate"_key, "Duplicate").clicked) {
///         ...
///     }
///     devgui::menuSeparator(ui);
///     if (devgui::menuItem(ui, "delete"_key, "Delete", {.shortcut = "Del"}).clicked) {
///         ...
///     }
/// }
/// ```
[[nodiscard]] MenuScope
contextMenu(UI& ui, const Response& response);

/// Returns the ID of the popup that `contextMenu()` opens for the widget id,
/// for example to ask `UI::isPointerOverPopup()` whether a press lands on it.
[[nodiscard]] WidgetID
getContextMenuID(WidgetID id) noexcept;

/// Builds the menu id below anchor while it is open, for example a menu that
/// a button opens with `UI::openPopup()`. The menu sizes itself to its items.
[[nodiscard]] MenuScope
popupMenu(UI& ui, WidgetID id, const F32Rect2D& anchor);

/// Adds an item that opens a submenu beside the menu being built, and builds
/// the submenu while it is open.
///
/// The submenu opens when the pointer rests on the item or presses it, and
/// closes when the pointer rests on another item of the same menu. End the
/// returned scope before the next item of the outer menu:
/// ```cpp
/// if (auto recent = devgui::subMenu(ui, "recent"_key, "Open Recent"); recent.isOpen()) {
///     ...
/// }
/// devgui::menuItem(ui, "save"_key, "Save");
/// ```
[[nodiscard]] MenuScope
subMenu(UI& ui, Key key, std::string_view label, bool enabled = true);

/// Adds an item to the menu being built.
///
/// `Response::clicked` is set when the user chose the item, which also closes
/// every open menu. A checked item shows a check mark; toggling the value it
/// stands for is up to the caller.
Response
menuItem(UI& ui, Key key, std::string_view label, const MenuItemOptions& options = {});

/// Adds a line between groups of menu items.
void menuSeparator(UI& ui);

} // namespace pomdog::devgui
