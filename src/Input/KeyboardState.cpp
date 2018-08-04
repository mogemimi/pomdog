// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Input/KeyboardState.hpp"
#include "Pomdog/Input/KeyState.hpp"
#include "Pomdog/Input/Keys.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace Pomdog {

static_assert(static_cast<std::underlying_type<Keys>::type>(Keys::None) == 0, "");
static_assert(std::is_same<std::underlying_type<Keys>::type, std::uint8_t>::value, "");
static_assert(std::is_unsigned<std::underlying_type<Keys>::type>::value,
    "Keys is unsinged integer type.");

KeyState KeyboardState::operator[](Keys key) const
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset.size());

    return keyset[static_cast<size_type>(key)] ?
        KeyState::Down: KeyState::Up;
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

} // namespace Pomdog
