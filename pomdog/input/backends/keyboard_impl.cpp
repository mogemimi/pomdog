// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"

namespace pomdog::detail {

KeyboardImpl::KeyboardImpl()
{
    keyboardState_.clearAllKeys();
}

bool KeyboardImpl::isKeyDown(Keys key) const noexcept
{
    return keyboardState_.isKeyDown(key);
}

bool KeyboardImpl::isKeyUp(Keys key) const noexcept
{
    return keyboardState_.isKeyUp(key);
}

bool KeyboardImpl::isControlKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftControl) || isKeyDown(Keys::RightControl);
}

bool KeyboardImpl::isShiftKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftShift) || isKeyDown(Keys::RightShift);
}

bool KeyboardImpl::isAltKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftAlt) || isKeyDown(Keys::RightAlt);
}

bool KeyboardImpl::isSuperKeyDown() const noexcept
{
    return isKeyDown(Keys::LeftSuper) || isKeyDown(Keys::RightSuper);
}

bool KeyboardImpl::isAnyKeyDown() const noexcept
{
    return keyboardState_.isAnyKeyDown();
}

std::string_view KeyboardImpl::getTextInput() const noexcept
{
    return textInput_;
}

void KeyboardImpl::setKey(Keys key, KeyState keyState) noexcept
{
    if (key == Keys::LeftSuper || key == Keys::RightSuper) {
        if (keyState == KeyState::Up) {
            // NOTE: Pressing the super key may cause the window to lose focus
            //       on Cocoa and Emscripten, and the KeyUp event will not be
            //       delivered, so the key may remain typed. To solve this
            //       problem, all key states are turned off when the super key
            //       is released.
            keyboardState_.clearAllKeys();
        }
    }

    keyboardState_.setKey(key, keyState);
}

void KeyboardImpl::appendTextInput(std::string_view text)
{
    textInput_.append(text);
}

void KeyboardImpl::clearAllKeys() noexcept
{
    keyboardState_.clearAllKeys();
}

void KeyboardImpl::clearTextInput() noexcept
{
    textInput_.clear();
}

} // namespace pomdog::detail
