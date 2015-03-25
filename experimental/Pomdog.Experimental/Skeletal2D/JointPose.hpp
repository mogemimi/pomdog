// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP
#define POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Radian.hpp"

namespace Pomdog {

class JointPose {
public:
	Vector2 Translate;
	Radian<float> Rotation;
	float Scale;
};

}// namespace Pomdog

#endif // !defined(POMDOG_JOINTPOSE_160B1EF6_234E_4107_ACA9_10FA3189A67C_HPP)
