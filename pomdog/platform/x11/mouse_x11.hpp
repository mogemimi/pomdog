// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/mouse.hpp"
#include "pomdog/input/mouse_state.hpp"
#include <X11/Xlib.h>

namespace Pomdog::Detail::X11 {

class MouseX11 final : public Mouse {
public:
    MouseState GetState() const override;

    void HandleEvent(XEvent& event);

private:
    MouseState mouseState;
};

} // namespace Pomdog::Detail::X11
