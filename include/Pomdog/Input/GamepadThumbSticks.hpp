// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GAMEPADTHUMBSTICKS_444752C9_1DEB_45F4_B42E_9212410AEE8D_HPP
#define POMDOG_GAMEPADTHUMBSTICKS_444752C9_1DEB_45F4_B42E_9212410AEE8D_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"

namespace Pomdog {

class GamepadThumbSticks {
public:
	Vector2 Left = Vector2::Zero;
	Vector2 Right = Vector2::Zero;
};

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEPADTHUMBSTICKS_444752C9_1DEB_45F4_B42E_9212410AEE8D_HPP)
