// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/backends/mouse_state.h"
#include "pomdog/input/mouse.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {

class MouseX11 final : public Mouse {
private:
    MouseState mouseState_;

public:
    [[nodiscard]] Point2D
    getPosition() const noexcept override;

    [[nodiscard]] bool
    isButtonDown(MouseButtons button) const noexcept override;

    [[nodiscard]] i32
    getScrollX() const noexcept override;

    [[nodiscard]] i32
    getScrollY() const noexcept override;

    [[nodiscard]] bool
    isPresent() const noexcept override;

    void handleEvent(XEvent& event);

    void clearAllButtons() noexcept;
};

} // namespace pomdog::detail::x11
