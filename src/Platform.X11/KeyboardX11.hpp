// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_KEYBOARDX11_68B433F3_HPP
#define POMDOG_KEYBOARDX11_68B433F3_HPP

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"
#include <X11/Xlib.h>
#include <array>

namespace Pomdog {
namespace Detail {
namespace X11 {

class KeyboardX11 final : public Keyboard {
public:
    explicit KeyboardX11(::Display* display);

    KeyboardState GetState() const override;

    void HandleEvent(XEvent & event);

private:
    std::array<Keys, 256> mappedKeys;
    KeyboardState keyboardState;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_KEYBOARDX11_68B433F3_HPP
