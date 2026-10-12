// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/devgui/key.h"
#include "pomdog/math/vector2.h"
#include "pomdog/memory/raw_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
class UI;
} // namespace pomdog::devgui

namespace pomdog::devgui {

/// Describes how a window first appears.
///
/// These values apply only when the window first appears. After that the user
/// moves, resizes, and collapses the window, and devgui keeps those changes.
struct WindowOptions final {
    /// The top-left corner in logical pixels.
    Vector2 position = Vector2{20.0f, 20.0f};

    /// The size in logical pixels, including the title bar.
    Vector2 size = Vector2{320.0f, 400.0f};

    /// True to show only the title bar.
    bool collapsed = false;
};

/// Keeps a window open until destroyed. Create one with `devgui::window()`.
class [[nodiscard]] WindowScope final {
public:
    WindowScope(UI& ui, Key key, std::string_view title, const WindowOptions& options, raw_ptr<bool> open);
    ~WindowScope();

    WindowScope(const WindowScope&) = delete;
    WindowScope& operator=(const WindowScope&) = delete;
    WindowScope(WindowScope&&) = delete;
    WindowScope& operator=(WindowScope&&) = delete;

    /// Returns true when the window's content is visible, that is, when the
    /// window is not collapsed. Skip building the content when this is false.
    [[nodiscard]] bool
    isOpen() const noexcept;

private:
    UI& ui_;
    bool open_ = false;
};

/// Opens a window that lasts until the returned object goes out of scope.
///
/// The user moves the window by its title bar, collapses it with the arrow,
/// and resizes it from the bottom-right corner. Pressing a window brings it to
/// the front. Content taller than the window scrolls. Windows cannot nest.
///
/// key names the window across the whole UI, whatever the current ID scope.
/// Dragging the window onto a dock space docks it there as a tab; see
/// `devgui::dockSpace()`.
/// ```cpp
/// if (auto window = devgui::window(ui, "stats"_key, "Stats"); window.isOpen()) {
///     devgui::text(ui, "...");
/// }
/// ```
[[nodiscard]] WindowScope
window(UI& ui, Key key, std::string_view title, const WindowOptions& options = {});

/// Opens a window like the overload above, with a close button on its title
/// bar and on its tab that sets open to false.
///
/// The application owns open and stops calling this while it is false. A
/// docked window keeps its place in the dock space while it is closed, and
/// takes it again when the application opens it:
/// ```cpp
/// if (statsOpen) {
///     if (auto window = devgui::window(ui, "stats"_key, "Stats", statsOpen); window.isOpen()) {
///         devgui::text(ui, "...");
///     }
/// }
/// ```
[[nodiscard]] WindowScope
window(UI& ui, Key key, std::string_view title, bool& open, const WindowOptions& options = {});

} // namespace pomdog::devgui
