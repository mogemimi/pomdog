//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP
#define POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Config/Export.hpp>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Radian.hpp>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay2D
/// @{

class POMDOG_EXPORT Transform2D {
public:
	Vector2 Position;
	Vector2 Scale;
	Radian<float> Rotation;
	
	Transform2D();
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_TRANSFORM2D_9973161D_D63B_499A_9158_F635629B7A26_HPP)
