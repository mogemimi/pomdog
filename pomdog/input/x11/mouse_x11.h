// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {

class MouseX11 final : public Mouse {
private:
    MouseState mouseState_;

public:
    [[nodiscard]] MouseState
    getState() const override;

    void handleEvent(XEvent& event);
};

} // namespace pomdog::detail::x11
