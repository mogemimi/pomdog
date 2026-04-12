// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/cocoa/keyboard_cocoa.h"
#include "pomdog/application/system_events.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail::cocoa {

KeyboardCocoa::KeyboardCocoa() = default;

bool KeyboardCocoa::isKeyDown(Keys key) const noexcept
{
    return state_.isKeyDown(key);
}

bool KeyboardCocoa::isKeyUp(Keys key) const noexcept
{
    return state_.isKeyUp(key);
}

bool KeyboardCocoa::isControlKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftControl) || isKeyDown(Keys::RightControl);
}

bool KeyboardCocoa::isShiftKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftShift) || isKeyDown(Keys::RightShift);
}

bool KeyboardCocoa::isAltKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftAlt) || isKeyDown(Keys::RightAlt);
}

bool KeyboardCocoa::isSuperKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftSuper) || isKeyDown(Keys::RightSuper);
}

bool KeyboardCocoa::isAnyKeyDown() const noexcept
{
    return state_.isAnyKeyDown();
}

void KeyboardCocoa::handleEvent(const SystemEvent& event)
{
    switch (event.kind) {
    case SystemEventKind::InputKeyEvent: {
        const auto ev = std::get<InputKeyEvent>(event.data);
        static_assert(sizeof(ev) <= 24);

        if (ev.key == Keys::LeftSuper || ev.key == Keys::RightSuper) {
            if (ev.state == KeyState::Up) {
                state_.clearAllKeys();
            }
        }

        state_.setKey(ev.key, ev.state);
        break;
    }
    case SystemEventKind::InputTextEvent: {
        const auto& ev = std::get<InputTextEvent>(event.data);
        textInput_ += ev.text;
        break;
    }
    default:
        break;
    }
}

std::string_view KeyboardCocoa::getTextInput() const noexcept
{
    return textInput_;
}

void KeyboardCocoa::clearAllKeys() noexcept
{
    state_.clearAllKeys();
}

void KeyboardCocoa::clearTextInput() noexcept
{
    textInput_.clear();
}

} // namespace pomdog::detail::cocoa
