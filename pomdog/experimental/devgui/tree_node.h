// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/experimental/devgui/response.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Describes how a tree node first appears.
struct TreeNodeOptions final {
    /// True to show the node open until the user first closes it.
    bool defaultOpen = false;
};

/// Keeps a tree node's ID scope and indentation until destroyed. Create one
/// with `devgui::treeNode()`.
class [[nodiscard]] TreeNodeScope final {
public:
    TreeNodeScope(UI& ui, Key key, bool open, const Response& response);
    ~TreeNodeScope();

    TreeNodeScope(const TreeNodeScope&) = delete;
    TreeNodeScope& operator=(const TreeNodeScope&) = delete;
    TreeNodeScope(TreeNodeScope&&) = delete;
    TreeNodeScope& operator=(TreeNodeScope&&) = delete;

    /// Returns true when the node is open and its children should be shown.
    [[nodiscard]] bool
    isOpen() const noexcept;

    /// Returns how the user interacted with the node's row, for example to
    /// give it a context menu.
    [[nodiscard]] const Response&
    getResponse() const noexcept;

private:
    UI& ui_;
    Response response_;
    bool open_ = false;
};

/// Displays a tree node row that the user opens and closes by clicking it.
///
/// The returned scope indents the widgets created while it lives and nests
/// their IDs under key, so sibling nodes can reuse the same child keys. The
/// open state persists across frames.
[[nodiscard]] TreeNodeScope
treeNode(UI& ui, Key key, std::string_view label, const TreeNodeOptions& options = {});

} // namespace pomdog::devgui
