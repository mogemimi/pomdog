// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/keyboard_state.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/key_state.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/enum_cast.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

static_assert(to_underlying(Keys::Unknown) == 0);
static_assert(std::is_same_v<std::underlying_type_t<Keys>, u8>);
static_assert(std::is_unsigned_v<std::underlying_type_t<Keys>>, "Keys is unsinged integer type.");

bool KeyboardState::isKeyDown(Keys key) const noexcept
{
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    return keyset_[to_underlying(key)];
}

bool KeyboardState::isKeyUp(Keys key) const noexcept
{
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    return !keyset_[to_underlying(key)];
}

void KeyboardState::setKey(Keys key, KeyState keyState) noexcept
{
    static_assert(to_underlying(KeyState::Up) == 0);
    static_assert(to_underlying(KeyState::Down) == 1);
    POMDOG_ASSERT(to_underlying(key) < keyset_.size());
    keyset_[to_underlying(key)] = (keyState != KeyState::Up);
}

void KeyboardState::clearAllKeys() noexcept
{
    keyset_.reset();
    POMDOG_ASSERT(keyset_.none());
}

} // namespace pomdog
