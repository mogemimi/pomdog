// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/input/mouse_buttons.h"
#include "pomdog/math/point2d.h"

namespace pomdog {

class POMDOG_EXPORT Mouse {
public:
    Mouse() noexcept;

    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;

    virtual ~Mouse();

    /// Gets the position of the mouse cursor.
    [[nodiscard]] virtual Point2D
    getPosition() const noexcept = 0;

    /// Determines whether the specified mouse button is pressed.
    [[nodiscard]] virtual bool
    isButtonDown(MouseButtons button) const noexcept = 0;

    /// Gets horizontal mouse scroll (accumulated).
    [[nodiscard]] virtual i32
    getScrollX() const noexcept = 0;

    /// Gets vertical mouse scroll (accumulated).
    [[nodiscard]] virtual i32
    getScrollY() const noexcept = 0;

    /// Returns true if a mouse is present, false otherwise.
    [[nodiscard]] virtual bool
    isPresent() const noexcept = 0;
};

} // namespace pomdog
