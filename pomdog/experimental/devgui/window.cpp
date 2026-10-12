// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/window.h"
#include "pomdog/experimental/devgui/ui.h"

namespace pomdog::devgui {

WindowScope::WindowScope(UI& ui, Key key, std::string_view title, const WindowOptions& options, raw_ptr<bool> open)
    : ui_(ui)
{
    const auto initialRect = F32Rect2D{options.position.x, options.position.y, options.size.x, options.size.y};
    open_ = ui_.beginWindow(key, title, initialRect, options.collapsed, open);
}

WindowScope::~WindowScope()
{
    ui_.endWindow();
}

bool WindowScope::isOpen() const noexcept
{
    return open_;
}

WindowScope window(UI& ui, Key key, std::string_view title, const WindowOptions& options)
{
    return WindowScope{ui, key, title, options, nullptr};
}

WindowScope window(UI& ui, Key key, std::string_view title, bool& open, const WindowOptions& options)
{
    return WindowScope{ui, key, title, options, &open};
}

} // namespace pomdog::devgui
