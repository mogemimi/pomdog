// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/keyboard.h"
#include "pomdog/input/keyboard_state.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <X11/Xlib.h>
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::x11 {

class KeyboardX11 final : public Keyboard {
private:
    std::array<Keys, 256> mappedKeys_;
    KeyboardState keyboardState_;

public:
    explicit KeyboardX11(::Display* display);

    [[nodiscard]] KeyboardState
    getState() const override;

    void handleEvent(XEvent& event, ::XIC inputContext);
};

} // namespace pomdog::detail::x11
