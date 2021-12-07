// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/keyboard.hpp"
#include "pomdog/input/keyboard_state.hpp"
#include <X11/Xlib.h>
#include <array>

namespace Pomdog::Detail::X11 {

class KeyboardX11 final : public Keyboard {
public:
    explicit KeyboardX11(::Display* display);

    KeyboardState GetState() const override;

    void HandleEvent(XEvent& event, ::XIC inputContext);

private:
    std::array<Keys, 256> mappedKeys;
    KeyboardState keyboardState;
};

} // namespace Pomdog::Detail::X11
