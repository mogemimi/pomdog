// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"
#include "pomdog/signals/signal.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class MouseState;

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    virtual ~Mouse();

    virtual MouseState GetState() const = 0;

    Signal<void(const Point2D& position)> Moved;

    Signal<void(MouseButtons mouseButton)> ButtonDown;

    Signal<void(MouseButtons mouseButton)> ButtonUp;

    Signal<void(std::int32_t delta)> ScrollWheel;
};

} // namespace pomdog
