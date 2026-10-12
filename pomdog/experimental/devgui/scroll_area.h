// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Keeps a scroll area open until destroyed. Create one with
/// `devgui::scrollArea()`.
class [[nodiscard]] ScrollAreaScope final {
public:
    ScrollAreaScope(UI& ui, Key key, f32 height);
    ~ScrollAreaScope();

    ScrollAreaScope(const ScrollAreaScope&) = delete;
    ScrollAreaScope& operator=(const ScrollAreaScope&) = delete;
    ScrollAreaScope(ScrollAreaScope&&) = delete;
    ScrollAreaScope& operator=(ScrollAreaScope&&) = delete;

private:
    UI& ui_;
    WidgetID id_ = {};
};

/// Opens a vertically scrolling area of the given height that spans the
/// current layout's width.
///
/// The widgets created until the returned object goes out of scope are clipped
/// to the area and nest their IDs under key. The scroll offset persists across
/// frames. The wheel scrolls the innermost scrollable area under the pointer.
[[nodiscard]] ScrollAreaScope
scrollArea(UI& ui, Key key, f32 height);

} // namespace pomdog::devgui
