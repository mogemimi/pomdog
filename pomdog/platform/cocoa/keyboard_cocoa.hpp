// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/input/keyboard.hpp"
#include "pomdog/input/keyboard_state.hpp"

namespace Pomdog::Detail {
class SystemEvent;
} // namespace Pomdog::Detail

namespace Pomdog::Detail::Cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void HandleEvent(const SystemEvent& event);

private:
    KeyboardState state;
};

} // namespace Pomdog::Detail::Cocoa
