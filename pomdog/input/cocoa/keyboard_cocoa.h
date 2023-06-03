// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/keyboard.h"
#include "pomdog/input/keyboard_state.h"

namespace pomdog::detail {
class SystemEvent;
} // namespace pomdog::detail

namespace pomdog::detail::cocoa {

class KeyboardCocoa final : public Keyboard {
private:
    KeyboardState state_;

public:
    KeyboardCocoa();

    [[nodiscard]] KeyboardState
    getState() const override;

    void handleEvent(const SystemEvent& event);
};

} // namespace pomdog::detail::cocoa
