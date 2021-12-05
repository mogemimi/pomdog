// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "KeyboardCocoa.hpp"
#include "../Application/SystemEvents.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::Cocoa {

KeyboardCocoa::KeyboardCocoa() = default;

KeyboardState KeyboardCocoa::GetState() const
{
    return state;
}

void KeyboardCocoa::HandleEvent(const SystemEvent& event)
{
    switch (event.Kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.Data);
        static_assert(sizeof(ev) <= 24);
        bool isKeyDown = state.IsKeyDown(ev.Key);

        state.SetKey(ev.Key, ev.State);

        switch (ev.State) {
        case KeyState::Down:
            if (!isKeyDown) {
                Keyboard::KeyDown(ev.Key);
            }
            break;
        case KeyState::Up:
            if (isKeyDown) {
                Keyboard::KeyUp(ev.Key);
            }
            break;
        }
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.Data);
        Keyboard::TextInput(ev.Text);
        break;
    }
    default:
        break;
    }
}

} // namespace Pomdog::Detail::Cocoa
