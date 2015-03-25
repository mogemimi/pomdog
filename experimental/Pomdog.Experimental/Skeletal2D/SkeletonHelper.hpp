// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP
#define POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "JointIndex.hpp"
#include "Pomdog/Math/Matrix3x2.hpp"
#include <vector>
#include <functional>

namespace Pomdog {

class Skeleton;
class SkeletonPose;
class Joint;

namespace SkeletonHelper {
	void Traverse(Skeleton const& skeleton, JointIndex const& jointIndex,
		std::function<void(Joint const&)> const& traverser);

	void Traverse(Skeleton const& skeleton,
		std::function<void(Joint const&)> const& traverser);

	void ToGlobalPose(Skeleton const& skeleton, SkeletonPose const& skeletonPose,
		std::vector<Matrix3x2> & globalPose);

	std::vector<Matrix3x2> ToGlobalPose(Skeleton const& skeleton, SkeletonPose const& skeletonPose);
}

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP)
