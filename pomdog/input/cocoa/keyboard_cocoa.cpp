// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/application/system_events.h"
#include "pomdog/input/key_state.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::cocoa {

KeyboardCocoa::KeyboardCocoa() = default;

KeyboardState KeyboardCocoa::GetState() const
{
    return state;
}

void KeyboardCocoa::HandleEvent(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.data);
        static_assert(sizeof(ev) <= 24);
        bool isKeyDown = state.IsKeyDown(ev.key);

        state.SetKey(ev.key, ev.state);

        switch (ev.state) {
        case KeyState::Down:
            if (!isKeyDown) {
                Keyboard::KeyDown(ev.key);
            }
            break;
        case KeyState::Up:
            if (isKeyDown) {
                Keyboard::KeyUp(ev.key);
            }
            break;
        }
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.data);
        Keyboard::TextInput(ev.text);
        break;
    }
    default:
        break;
    }
}

} // namespace pomdog::detail::cocoa
