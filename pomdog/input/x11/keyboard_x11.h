// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/keyboard.h"
#include "pomdog/input/keyboard_state.h"
#include <X11/Xlib.h>
#include <array>

namespace pomdog::detail::x11 {

class KeyboardX11 final : public Keyboard {
public:
    explicit KeyboardX11(::Display* display);

    KeyboardState GetState() const override;

    void HandleEvent(XEvent& event, ::XIC inputContext);

private:
    std::array<Keys, 256> mappedKeys;
    KeyboardState keyboardState;
};

} // namespace pomdog::detail::x11
