//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Input/KeyboardState.hpp>
#include <type_traits>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Input/Keys.hpp>
#include <Pomdog/Input/KeyState.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
static_assert(static_cast<typename std::underlying_type<Keys>::type>(Keys::None) == 0, "");
static_assert(std::is_same<typename std::underlying_type<Keys>::type, std::uint8_t>::value, "");

static_assert(std::is_unsigned<typename std::underlying_type<Keys>::type>::value,
		"Keys is unsinged integer type.");
//-----------------------------------------------------------------------
KeyState KeyboardState::operator[](Keys key) const
{
	using size_type = typename std::underlying_type<decltype(key)>::type;
	POMDOG_ASSERT(keyset.size() > static_cast<size_type>(key));
	
	return keyset[static_cast<size_type>(key)] ?
		KeyState::Down: KeyState::Up;
}
//-----------------------------------------------------------------------
bool KeyboardState::IsKeyDown(Keys key) const
{
	using size_type = typename std::underlying_type<decltype(key)>::type;
	POMDOG_ASSERT(keyset.size() > static_cast<size_type>(key));
	
	return keyset[static_cast<size_type>(key)];
}
//-----------------------------------------------------------------------
bool KeyboardState::IsKeyUp(Keys key) const
{
	using size_type = typename std::underlying_type<decltype(key)>::type;
	POMDOG_ASSERT(keyset.size() > static_cast<size_type>(key));
	
	return !keyset[static_cast<size_type>(key)];
}
//-----------------------------------------------------------------------
void KeyboardState::SetKey(Keys key, KeyState keyState)
{
	using size_type = typename std::underlying_type<decltype(key)>::type;
	POMDOG_ASSERT(keyset.size() > static_cast<size_type>(key));

	static_assert(static_cast<bool>(KeyState::Down) == true, "");
	
	keyset[static_cast<size_type>(key)] = (keyState == KeyState::Down);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
