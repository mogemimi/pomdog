//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP
#define POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"
#include "Pomdog/Gameplay/Component.hpp"

namespace Pomdog {

class POMDOG_EXPORT Transform2D: public Component<Transform2D> {
public:
	Vector2 Position {0, 0};
	Vector2 Scale {1, 1};
	Radian<float> Rotation {0};
};

}// namespace Pomdog

#endif // !defined(POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP)
