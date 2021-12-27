// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/mouse.h"
#include "pomdog/input/mouse_state.h"
#include <X11/Xlib.h>

namespace pomdog::detail::x11 {

class MouseX11 final : public Mouse {
public:
    MouseState GetState() const override;

    void HandleEvent(XEvent& event);

private:
    MouseState mouseState;
};

} // namespace pomdog::detail::x11
