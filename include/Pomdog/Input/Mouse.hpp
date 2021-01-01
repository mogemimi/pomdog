// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Input/MouseButtons.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include "Pomdog/Signals/Signal.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

} // namespace Pomdog
