//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP
#define POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <functional>
#include "JointIndex.hpp"

namespace Pomdog {

class Skeleton;
class SkeletonPose;
class Joint;

namespace SkeletonHelper {
	void Traverse(Skeleton const& skeleton, JointIndex const& jointIndex,
		std::function<void(Joint const&)> const& traverser);

	void Traverse(Skeleton const& skeleton,
		std::function<void(Joint const&)> const& traverser);
		
	void ComputeGlobalPoseFromLocalPose(Skeleton const& skeleton,
		SkeletonPose & skeletonPose);
}

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETONHELPER_16AC5743_D6BE_4B86_AB0B_04AD6506605B_HPP)
