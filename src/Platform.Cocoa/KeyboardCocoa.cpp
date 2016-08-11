// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "KeyboardCocoa.hpp"
#include "Pomdog/Input/KeyState.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

KeyboardCocoa::KeyboardCocoa() = default;

KeyboardState KeyboardCocoa::GetState() const
{
    return state;
}

void KeyboardCocoa::SetKey(Keys key, KeyState keyState)
{
    bool isKeyDown = state.IsKeyDown(key);

    state.SetKey(key, keyState);

    switch (keyState) {
    case KeyState::Down:
        if (!isKeyDown) {
            Keyboard::KeyDown(key);
        }
        break;
    case KeyState::Up:
        if (isKeyDown) {
            Keyboard::KeyUp(key);
        }
        break;
    }
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
