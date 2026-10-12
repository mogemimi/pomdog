// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/list_view.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

ListViewScope::ListViewScope(UI& ui, Key key, const ListViewOptions& options)
    : ui_(ui)
    , reorderable_(options.reorderable)
{
    ui_.pushIDScope(key);

    // NOTE: Like the lists of VS Code, rows have no space between them.
    ui_.beginVertical(0.0f);
}

ListViewScope::~ListViewScope()
{
    if (draggedIndex_ >= 0) {
        // NOTE: Draw the line after the rows, so that it lies on top of them.
        detail::drawDropIndicator(ui_, firstRect_, gapY_, false);
    }
    if (droppedIndex_ >= 0) {
        // NOTE: The rows were drawn in the old order. Ask for a frame that
        // draws them in the new one.
        ui_.requestUpdateAt(ui_.getInput().timeSeconds);
    }
    ui_.endVertical();
    ui_.popIDScope();
}

Response ListViewScope::item(Key key, std::string_view label, const ListItemOptions& options)
{
    const auto& style = ui_.getStyle();
    const auto& input = ui_.getInput();
    const auto index = count_;
    ++count_;

    const auto rect = ui_.allocate(0.0f, style.getFrameHeight());
    const auto response = ui_.interactBehind(ui_.makeID(key), rect);
    if (index == 0) {
        firstRect_ = rect;
        gapY_ = rect.y;
    }

    // NOTE: The drop goes to the edge between rows nearest the pointer, which
    // lies below every row whose middle is above the pointer.
    if (input.pointerPosition.y >= rect.y + rect.height * 0.5f) {
        gap_ = index + 1;
        gapY_ = rect.y + rect.height;
    }

    bool dragging = false;
    if (reorderable_ && response.isHeld()) {
        auto& travel = ui_.getActiveScratch();
        if (!response.activated) {
            const auto delta = ui_.getPointerDelta();
            travel += std::abs(delta.x) + std::abs(delta.y);
        }
        if (travel >= detail::reorderDragThreshold) {
            if (response.active) {
                draggedIndex_ = index;
                dragging = true;
            }
            else {
                droppedIndex_ = index;
            }
        }
    }

    if (options.selected) {
        ui_.fillRect(rect, style.rowSelectedColor);
    }
    else if (response.hovered || response.active) {
        ui_.fillRect(rect, style.rowHoveredColor);
    }

    const auto labelWidth = std::ceil(ui_.measureText(label));
    ui_.requestWidth(labelWidth + style.framePadding.x * 2.0f + options.trailingWidth);
    const auto textX = rect.x + style.framePadding.x;
    const auto maxWidth = std::max(0.0f, rect.width - style.framePadding.x * 2.0f - options.trailingWidth);
    ui_.drawText(Vector2{textX, detail::getTextTop(style, rect)}, label, style.textColor, maxWidth);

    if (dragging) {
        // NOTE: The copy keeps the point where the user grabbed the row under
        // the pointer.
        const auto grab = ui_.getPressPosition() - Vector2{rect.x, rect.y};
        const auto ghost = F32Rect2D{input.pointerPosition.x - grab.x, input.pointerPosition.y - grab.y, rect.width, rect.height};
        detail::drawDragGhost(ui_, ghost, label, style.framePadding.x);
    }
    return response;
}

std::optional<ListMove> ListViewScope::getMove() const noexcept
{
    if (droppedIndex_ < 0) {
        return std::nullopt;
    }
    const auto to = getDropPosition(droppedIndex_);
    if (to == droppedIndex_) {
        return std::nullopt;
    }
    return ListMove{.from = droppedIndex_, .to = to};
}

i32 ListViewScope::getDropPosition(i32 from) const noexcept
{
    // NOTE: Removing the row shifts the edges below it up by one.
    return (gap_ > from) ? (gap_ - 1) : gap_;
}

ListViewScope listView(UI& ui, Key key, const ListViewOptions& options)
{
    return ListViewScope{ui, key, options};
}

} // namespace pomdog::devgui
