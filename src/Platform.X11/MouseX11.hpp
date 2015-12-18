// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_MOUSEX11_9FA99050_HPP
#define POMDOG_MOUSEX11_9FA99050_HPP

#include "Pomdog/Input/Mouse.hpp"
#include "Pomdog/Input/MouseState.hpp"
#include <X11/Xlib.h>

namespace Pomdog {
namespace Detail {
namespace X11 {

class MouseX11 final : public Mouse {
public:
    MouseState GetState() const override;

    void HandleEvent(XEvent & event);

private:
    MouseState mouseState;
};

} // namespace X11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_MOUSEX11_9FA99050_HPP
