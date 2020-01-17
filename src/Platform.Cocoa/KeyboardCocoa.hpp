// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {

class Event;

} // namespace Pomdog

namespace Pomdog::Detail::Cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void HandleEvent(const Event& event);

private:
    KeyboardState state;
};

} // namespace Pomdog::Detail::Cocoa
