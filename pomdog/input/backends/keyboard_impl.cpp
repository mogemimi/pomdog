// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/backends/keyboard_impl.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/enum_cast.h"

namespace pomdog::detail {

static_assert(to_underlying(Keys::Unknown) == 0);
static_assert(std::is_same_v<std::underlying_type_t<Keys>, u8>);
static_assert(std::is_unsigned_v<std::underlying_type_t<Keys>>);

KeyboardImpl::KeyboardImpl()
{
    keyset_.reset();
}

bool KeyboardImpl::isKeyDown(Keys key) const noexcept
{
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    return keyset_[to_underlying(key)];
}

bool KeyboardImpl::isKeyUp(Keys key) const noexcept
{
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    return !keyset_[to_underlying(key)];
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
    return keyset_.any();
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
            keyset_.reset();
        }
    }

    static_assert(to_underlying(KeyState::Up) == 0);
    static_assert(to_underlying(KeyState::Down) == 1);
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    keyset_[to_underlying(key)] = (keyState != KeyState::Up);
}

void KeyboardImpl::appendTextInput(std::string_view text)
{
    textInput_.append(text);
}

void KeyboardImpl::clearAllKeys() noexcept
{
    keyset_.reset();
}

void KeyboardImpl::clearTextInput() noexcept
{
    textInput_.clear();
}

} // namespace pomdog::detail
