// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/keyboard_state.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/input/key_state.hpp"
#include "pomdog/input/keys.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

static_assert(static_cast<std::underlying_type_t<Keys>>(Keys::Unknown) == 0);
static_assert(static_cast<std::underlying_type_t<Keys>>(Keys::Cancel) == 1);
static_assert(static_cast<std::underlying_type_t<Keys>>(Keys::Help) == 2);
static_assert(static_cast<std::underlying_type_t<Keys>>(Keys::CloseBracket) == 140);
static_assert(static_cast<std::underlying_type_t<Keys>>(Keys::Quote) == 141);
static_assert(std::is_same_v<std::underlying_type_t<Keys>, std::uint8_t>);
static_assert(std::is_unsigned_v<std::underlying_type_t<Keys>>, "Keys is unsinged integer type.");

KeyState KeyboardState::operator[](Keys key) const
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset.size());

    return keyset[static_cast<size_type>(key)] ? KeyState::Down : KeyState::Up;
}

bool KeyboardState::IsKeyDown(Keys key) const
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset.size());

    return keyset[static_cast<size_type>(key)];
}

bool KeyboardState::IsKeyUp(Keys key) const
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset.size());

    return !keyset[static_cast<size_type>(key)];
}

void KeyboardState::SetKey(Keys key, KeyState keyState)
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset.size());

    static_assert(static_cast<bool>(KeyState::Down) == true, "");

    keyset[static_cast<size_type>(key)] = (keyState == KeyState::Down);
}

void KeyboardState::ClearAllKeys()
{
    keyset.reset();
    POMDOG_ASSERT(keyset.none());
}

} // namespace pomdog