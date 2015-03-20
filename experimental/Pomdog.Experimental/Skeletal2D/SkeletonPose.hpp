// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP
#define POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "JointPose.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>

namespace Pomdog {

class Skeleton;

class SkeletonPose {
public:
	std::vector<JointPose> JointPoses;

	static SkeletonPose CreateBindPose(Skeleton const& skeleton);
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONPOSE_A5B94631_577E_4148_A8F7_DEBCE99A0218_HPP)
