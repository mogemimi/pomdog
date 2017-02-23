// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {

class Event;

} // namespace Pomdog

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void HandleEvent(const Event& event);

private:
    KeyboardState state;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
