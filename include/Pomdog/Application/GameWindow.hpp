// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEWINDOW_22D747BC_HPP
#define POMDOG_GAMEWINDOW_22D747BC_HPP

#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <string>
#include <cstdint>

namespace Pomdog {

class Rectangle;
enum class MouseCursor : std::uint8_t;

class POMDOG_EXPORT GameWindow {
public:
    GameWindow() = default;
    GameWindow(GameWindow const&) = delete;
    GameWindow & operator=(GameWindow const&) = delete;

    virtual ~GameWindow() = default;

    virtual bool GetAllowUserResizing() const = 0;

    virtual void SetAllowUserResizing(bool allowResizing) = 0;

    virtual std::string GetTitle() const = 0;

    virtual void SetTitle(std::string const& title) = 0;

    virtual Rectangle GetClientBounds() const = 0;

    virtual void SetClientBounds(Rectangle const& clientBounds) = 0;

    ///@brief Return true if the mouse cursor is visible, false otherwise.
    virtual bool IsMouseCursorVisible() const = 0;

    virtual void SetMouseCursorVisible(bool visible) = 0;

    virtual void SetMouseCursor(MouseCursor cursor) = 0;

    Signal<void(int width, int height)> ClientSizeChanged;
};

} // namespace Pomdog

#endif // POMDOG_GAMEWINDOW_22D747BC_HPP
