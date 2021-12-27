// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/keyboard.h"
#include "pomdog/input/keyboard_state.h"

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void HandleEvent(const SystemEvent& event);

private:
    KeyboardState state;
};

} // namespace pomdog::detail::cocoa
