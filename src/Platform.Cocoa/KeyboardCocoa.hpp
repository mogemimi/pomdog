// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Input/Keyboard.hpp"
#include "Pomdog/Input/KeyboardState.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

class KeyboardCocoa final : public Keyboard {
public:
    KeyboardCocoa();

    KeyboardState GetState() const override;

    void SetKey(Keys key, KeyState keyState);

private:
    KeyboardState state;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
