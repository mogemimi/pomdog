//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "KeyboardCocoa.hpp"

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog
