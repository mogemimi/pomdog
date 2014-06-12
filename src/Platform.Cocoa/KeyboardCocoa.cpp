//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "KeyboardCocoa.hpp"

namespace Pomdog {
namespace Details {
namespace Cocoa {

KeyboardCocoa::KeyboardCocoa()
{
}
//-----------------------------------------------------------------------
KeyboardState const& KeyboardCocoa::State() const
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
