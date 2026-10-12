// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/tree_node.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {

TreeNodeScope::TreeNodeScope(UI& ui, Key key, bool open, const Response& response)
    : ui_(ui)
    , response_(response)
    , open_(open)
{
    ui_.pushIDScope(key);
    if (open_) {
        ui_.indent(ui_.getStyle().indent);
    }
}

TreeNodeScope::~TreeNodeScope()
{
    if (open_) {
        ui_.indent(-ui_.getStyle().indent);
    }
    ui_.popIDScope();
}

bool TreeNodeScope::isOpen() const noexcept
{
    return open_;
}

const Response& TreeNodeScope::getResponse() const noexcept
{
    return response_;
}

TreeNodeScope treeNode(UI& ui, Key key, std::string_view label, const TreeNodeOptions& options)
{
    const auto& style = ui.getStyle();
    const auto id = ui.makeID(key);

    bool open = options.defaultOpen;
    if (const auto stored = ui.loadState(id); stored) {
        open = (*stored != 0);
    }

    const auto rect = ui.allocate(0.0f, style.getFrameHeight());
    const auto response = ui.interact(id, rect);
    if (response.clicked) {
        open = !open;
        ui.storeState(id, open ? 1 : 0);
    }

    if (response.active) {
        ui.fillRect(rect, style.rowSelectedColor, style.frameRounding);
    }
    else if (response.hovered) {
        ui.fillRect(rect, style.rowHoveredColor, style.frameRounding);
    }

    const auto chevronSize = std::round(style.fontSize * 0.22f);
    const auto center = Vector2{rect.x + rect.height * 0.5f, rect.y + rect.height * 0.5f};
    const auto direction = open ? detail::ChevronDirection::Down : detail::ChevronDirection::Right;
    detail::drawChevron(ui, center, chevronSize, direction, style.textColor);

    const auto textX = rect.x + rect.height;
    ui.drawText(Vector2{textX, detail::getTextTop(style, rect)}, label, style.textColor, rect.x + rect.width - textX);
    return TreeNodeScope{ui, key, open, response};
}

} // namespace pomdog::devgui
