// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/signals/signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class Rectangle;
enum class MouseCursor : std::uint8_t;

/// Base interface for application window that contains the game.
///
/// Instances of this class are unique.
///
/// @todo ClientSizeChanged should fire in Cocoa and Win32 backends.
class POMDOG_EXPORT GameWindow {
public:
    /// Constructs plaftorm-default GameWindow.
    GameWindow();
    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    virtual ~GameWindow();

    /// @return If window can be resized by user.
    virtual bool GetAllowUserResizing() const = 0;

    /// Set if window can be resized by user.
    /// @param allowResizing New value of the resizing flag.
    virtual void SetAllowUserResizing(bool allowResizing) = 0;

    /// @return Title of the window.
    virtual std::string GetTitle() const = 0;

    /// Set title of the window.
    /// @param title New title of the window.
    virtual void SetTitle(const std::string& title) = 0;

    /// @return Rectangle that describes windows position and size.
    virtual Rectangle GetClientBounds() const = 0;

    /// Moves the window to the @p clientBounds x and y, and sets windows
    /// width and height according to the method parameter.
    /// @param clientBounds Rectangle that will adjust the window.
    virtual void SetClientBounds(const Rectangle& clientBounds) = 0;

    /// @return True if the mouse cursor is visible, false otherwise.
    virtual bool IsMouseCursorVisible() const = 0;

    /// Set visibility of the cursor in the window
    /// @param visible New value of the cursor visibility flag.
    virtual void SetMouseCursorVisible(bool visible) = 0;

    /// Set mouse cursor system provided asset.
    /// @param cursor New mouse cursor asset.
    virtual void SetMouseCursor(MouseCursor cursor) = 0;

    /// Signal that fires when windows size is changed.
    /// @warning Do not fire in Cocoa and Win32 !
    Signal<void(int width, int height)> ClientSizeChanged;
};

} // namespace Pomdog
