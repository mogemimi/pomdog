// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

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
    GameWindow(const GameWindow&) = delete;
    GameWindow & operator=(const GameWindow&) = delete;

    virtual ~GameWindow() = default;

    virtual bool GetAllowUserResizing() const = 0;

    virtual void SetAllowUserResizing(bool allowResizing) = 0;

    virtual std::string GetTitle() const = 0;

    virtual void SetTitle(const std::string& title) = 0;

    virtual Rectangle GetClientBounds() const = 0;

    virtual void SetClientBounds(const Rectangle& clientBounds) = 0;

    ///@brief Return true if the mouse cursor is visible, false otherwise.
    virtual bool IsMouseCursorVisible() const = 0;

    virtual void SetMouseCursorVisible(bool visible) = 0;

    virtual void SetMouseCursor(MouseCursor cursor) = 0;

    Signal<void(int width, int height)> ClientSizeChanged;
};

} // namespace Pomdog
