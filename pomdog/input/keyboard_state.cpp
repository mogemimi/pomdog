// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/input/keyboard_state.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/input/key_state.h"
#include "pomdog/input/keys.h"
#include "pomdog/utility/assert.h"

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

bool KeyboardState::isKeyDown(Keys key) const noexcept
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset_.size());

    return keyset_[static_cast<size_type>(key)];
}

bool KeyboardState::isKeyUp(Keys key) const noexcept
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset_.size());

    return !keyset_[static_cast<size_type>(key)];
}

void KeyboardState::setKey(Keys key, KeyState keyState) noexcept
{
    using size_type = std::underlying_type<decltype(key)>::type;
    POMDOG_ASSERT(static_cast<size_type>(key) < keyset_.size());

    static_assert(static_cast<bool>(KeyState::Down) == true, "");

    keyset_[static_cast<size_type>(key)] = (keyState == KeyState::Down);
}

void KeyboardState::clearAllKeys() noexcept
{
    keyset_.reset();
    POMDOG_ASSERT(keyset_.none());
}

} // namespace pomdog
