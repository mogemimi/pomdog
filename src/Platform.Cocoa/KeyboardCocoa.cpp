// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "KeyboardCocoa.hpp"

namespace Pomdog {
namespace Detail {
namespace Cocoa {

KeyboardCocoa::KeyboardCocoa() = default;
//-----------------------------------------------------------------------
KeyboardState KeyboardCocoa::GetState() const
{
	return state;
}
//-----------------------------------------------------------------------
void KeyboardCocoa::SetKey(Keys key, KeyState keyState)
{
	state.SetKey(key, keyState);
}

}// namespace Cocoa
}// namespace Detail
}// namespace Pomdog
