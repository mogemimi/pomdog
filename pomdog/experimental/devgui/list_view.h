// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"
#include "pomdog/math/f32_rect2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Describes how a list view behaves.
struct ListViewOptions final {
    /// True to let the user reorder the rows by dragging them.
    bool reorderable = true;
};

/// Describes one row of a list view.
struct ListItemOptions final {
    /// True to draw the row as selected. The selection belongs to the caller.
    bool selected = false;

    /// The width at the right end of the row that the label leaves free for
    /// widgets the caller places there, such as a close button.
    f32 trailingWidth = 0.0f;
};

/// Reports a row that the user dragged to another position.
///
/// Move the item at from to to, shifting the items between by one toward
/// from, such as with `std::rotate()`.
struct ListMove final {
    /// The position of the row before the move.
    i32 from = 0;

    /// The position of the row after the move.
    i32 to = 0;
};

/// Builds the rows of a list view until destroyed. Create one with
/// `devgui::listView()`.
class [[nodiscard]] ListViewScope final {
public:
    ListViewScope(UI& ui, Key key, const ListViewOptions& options);
    ~ListViewScope();

    ListViewScope(const ListViewScope&) = delete;
    ListViewScope& operator=(const ListViewScope&) = delete;
    ListViewScope(ListViewScope&&) = delete;
    ListViewScope& operator=(ListViewScope&&) = delete;

    /// Adds the next row, showing label, and returns how the user interacted
    /// with it.
    ///
    /// key names the row within the list; use a stable model ID for its index
    /// so that a row keeps its state when the rows move. The row lies behind
    /// widgets placed over it afterward, such as a close button in its
    /// trailing width, so the press reaches them.
    Response
    item(Key key, std::string_view label, const ListItemOptions& options = {});

    /// Returns the move the user made by dropping a dragged row in this
    /// frame, if any.
    ///
    /// Call it after the last row and apply the move to the items, so that
    /// the next frame lists them in the new order.
    [[nodiscard]] std::optional<ListMove>
    getMove() const noexcept;

private:
    [[nodiscard]] i32
    getDropPosition(i32 from) const noexcept;

private:
    UI& ui_;
    F32Rect2D firstRect_ = {0.0f, 0.0f, 0.0f, 0.0f};

    /// The edge between rows nearest the pointer: the number of rows above
    /// it, and its position.
    i32 gap_ = 0;
    f32 gapY_ = 0.0f;

    i32 count_ = 0;

    /// The row the pointer drags in this frame, or -1.
    i32 draggedIndex_ = -1;

    /// The row the pointer dropped in this frame, or -1.
    i32 droppedIndex_ = -1;
    bool reorderable_ = true;
};

/// Starts a list view whose rows are added with `ListViewScope::item()`.
///
/// The list draws rows right next to each other and reports how the user
/// interacts with them; the items, their order, and the selection stay with
/// the caller. Dragging a row shows a translucent copy under the pointer and a
/// line where the row will go, and dropping it reports a move. Each row's
/// response lets the caller add widgets over the row and show a tooltip:
/// ```cpp
/// auto list = devgui::listView(ui, "editors"_key);
/// for (const auto& editor : editors) {
///     const auto options = devgui::ListItemOptions{.selected = (editor.id == current), .trailingWidth = 20.0f};
///     const auto row = list.item("editor"_key.at(editor.id), editor.name, options);
///     if (row.clicked) {
///         current = editor.id;
///     }
///     if (row.hovered || options.selected) {
///         const auto closeRect = F32Rect2D{row.rect.x + row.rect.width - 20.0f, row.rect.y, 20.0f, row.rect.height};
///         if (devgui::closeButton(ui, "close"_key.at(editor.id), closeRect).clicked) {
///             ...
///         }
///     }
///     devgui::tooltip(ui, row, editor.path);
/// }
/// if (const auto move = list.getMove(); move) {
///     moveItem(editors, move->from, move->to);
/// }
/// ```
[[nodiscard]] ListViewScope
listView(UI& ui, Key key, const ListViewOptions& options = {});

} // namespace pomdog::devgui
