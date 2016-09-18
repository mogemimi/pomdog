// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Signals/Signal.hpp"
#include "Pomdog/Basic/Export.hpp"

namespace Pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse & operator=(const Mouse&) = delete;

    virtual ~Mouse() = default;

    virtual MouseState GetState() const = 0;

//    Signal<void(MouseEvent const& mouseEvent)> MouseMove;
//
//    Signal<void(MouseEvent const& mouseEvent)> ButtonDown;
//
//    Signal<void(MouseEvent const& mouseEvent)> ButtonUp;
//
//    Signal<void(MouseEvent const& mouseEvent)> Click;
//
//    Signal<void(MouseEvent const& mouseEvent)> Wheel;
};

} // namespace Pomdog
