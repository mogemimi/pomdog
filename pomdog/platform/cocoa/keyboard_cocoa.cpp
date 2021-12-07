// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/keyboard_cocoa.hpp"
#include "pomdog/application/system_events.hpp"
#include "pomdog/input/key_state.hpp"
#include "pomdog/utility/assert.hpp"

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
