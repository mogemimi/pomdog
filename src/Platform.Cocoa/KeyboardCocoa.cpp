// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "KeyboardCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Signals/Event.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

KeyboardCocoa::KeyboardCocoa() = default;

KeyboardState KeyboardCocoa::GetState() const
{
    return state;
}

void KeyboardCocoa::HandleEvent(const Event& event)
{
    if (auto keyEvent = event.As<InputKeyEvent>()) {
        const auto key = keyEvent->Key;
        const auto keyState = keyEvent->State;

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
    else if (auto inputTextEvent = event.As<InputTextEvent>()) {
        Keyboard::TextInput(inputTextEvent->text);
    }
}

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
