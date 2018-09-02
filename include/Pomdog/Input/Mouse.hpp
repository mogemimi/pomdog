// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/MouseButtons.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Signals/Signal.hpp"
#include <cstdint>

namespace Pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    virtual ~Mouse() = default;

    virtual MouseState GetState() const = 0;

    Signal<void(const Point2D& position)> Moved;

    Signal<void(MouseButtons mouseButton)> ButtonDown;

    Signal<void(MouseButtons mouseButton)> ButtonUp;

    Signal<void(std::int32_t delta)> ScrollWheel;

    //Signal<void(MouseButtons mouseButton)> Click;
};

} // namespace Pomdog
