// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"
#include "pomdog/signals/signal.h"

namespace pomdog {
class MouseState;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT Mouse {
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    virtual ~Mouse();

    [[nodiscard]] virtual MouseState
    getState() const = 0;

    Signal<void(const Point2D& position)> Moved;

    Signal<void(MouseButtons mouseButton)> ButtonDown;

    Signal<void(MouseButtons mouseButton)> ButtonUp;

    Signal<void(std::int32_t delta)> ScrollWheel;
};

} // namespace pomdog
