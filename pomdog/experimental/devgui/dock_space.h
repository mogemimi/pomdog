// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/math/f32_rect2d.h"

namespace pomdog::devgui {
class Context;
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Selects how a dock node divides its area between its two children.
enum class DockSplit : u8 {
    /// Places the first child to the left of the second.
    LeftRight,

    /// Places the first child above the second.
    TopBottom,
};

/// Identifies a node in the layout tree of a dock space.
///
/// It stays valid until the tree changes shape, so use it only while building
/// a layout or within the frame that returned it.
struct DockNodeID final {
    i32 index = -1;

    [[nodiscard]] constexpr bool
    operator==(const DockNodeID& other) const noexcept = default;
};

/// Describes how a dock space shows its nodes in one frame.
struct DockSpaceOptions final {
    /// False to hide the tab bars, for example while the application lists
    /// the docked windows elsewhere, such as in a list view. Each node then
    /// shows its selected window in its whole area.
    bool showTabBars = true;
};

/// Describes one window docked in a dock space. See
/// `Context::getDockedWindows()`.
struct DockedWindow final {
    /// The key the window was created with.
    Key window = {};

    /// The leaf node the window is docked in. Windows of one node share it.
    DockNodeID node = {};

    /// True when the window is the selected tab of its node.
    bool selected = false;
};

/// Holds the two children that splitting a dock node creates.
struct DockNodePair final {
    DockNodeID first = {};
    DockNodeID second = {};
};

/// Builds the layout of a dock space, such as an editor's default layout.
///
/// Creating a builder replaces the dock space's layout with one empty node.
/// Split nodes to divide the area, then dock windows into the leaf nodes as
/// tabs. Windows are named by the keys they are created with, so the layout
/// can be built before the windows first appear:
/// ```cpp
/// devgui::DockLayoutBuilder layout{context, "editor"_key};
/// const auto [left, right] = layout.split(layout.root(), devgui::DockSplit::LeftRight, 0.25f);
/// layout.dock(left, "hierarchy"_key);
/// layout.dock(right, "scene"_key);
/// layout.dock(right, "game"_key);
/// ```
/// The first window docked into a node becomes its selected tab.
class DockLayoutBuilder final {
public:
    DockLayoutBuilder(Context& context, Key dockSpaceKey);

    DockLayoutBuilder(const DockLayoutBuilder&) = delete;
    DockLayoutBuilder& operator=(const DockLayoutBuilder&) = delete;

    /// Returns the node that covers the whole dock space.
    [[nodiscard]] DockNodeID
    root() const noexcept;

    /// Divides the leaf node into two children and returns them. ratio is the
    /// first child's share of the area. Tabs already docked in node move to the
    /// first child.
    [[nodiscard]] DockNodePair
    split(DockNodeID node, DockSplit split, f32 ratio);

    /// Docks the window windowKey into the leaf node as its last tab, removing
    /// it from any node it was docked in.
    void
    dock(DockNodeID node, Key windowKey);

private:
    Context& context_;
    WidgetID dockSpaceID_ = {};
};

/// Fills rect with a dock space, an area where windows dock as tabs and
/// splits.
///
/// Call it every frame at the root of the frame, outside any window and popup,
/// before the windows it hosts. Its layout persists across frames. The user
/// resizes the splits by dragging the gaps between them, selects windows by
/// their tabs, reorders tabs by dragging them along the tab bar, drags a tab
/// away to float its window, and drops a floating window onto a node to dock
/// it there: in the middle of the node as a tab, or near an edge as a split.
/// A docked window that the application stops submitting hides its tab, and
/// takes its place again when it comes back. A window that the user can close
/// shows a close button on its tab.
void dockSpace(UI& ui, Key key, const F32Rect2D& rect, const DockSpaceOptions& options = {});

} // namespace pomdog::devgui
