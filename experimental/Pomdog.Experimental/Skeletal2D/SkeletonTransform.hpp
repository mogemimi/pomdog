//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SKELETONTRANSFORM_B5648A89_636D_47D9_A280_946513E743EF_HPP
#define POMDOG_SKELETONTRANSFORM_B5648A89_636D_47D9_A280_946513E743EF_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "SkeletonPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog {

class SkeletonTransform {
public:
	SkeletonPose Pose;
	std::vector<Matrix3x2> GlobalPose;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONTRANSFORM_B5648A89_636D_47D9_A280_946513E743EF_HPP)
