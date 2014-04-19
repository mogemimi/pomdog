//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP
#define POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <string>
#include <Pomdog/Pomdog.hpp>
#include "JointIndex.hpp"

namespace Pomdog {

class Joint {
public:
	Vector2 Translate;
	Radian<float> Rotation;
	float Scale;

	JointIndex Index;
	JointIndex Parent;
	JointIndex FirstChild;
	JointIndex Sibling;
};

}// namespace Pomdog

#endif // !defined(POMDOG_JOINT_0CE2C1D3_6959_40C5_B01B_7FA838BCB51B_HPP)
