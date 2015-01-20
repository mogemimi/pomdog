//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SkeletonPose.hpp"
#include "Skeleton.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
SkeletonPose SkeletonPose::CreateBindPose(Skeleton const& skeleton)
{
	SkeletonPose skeletonPose;
	skeletonPose.JointPoses.reserve(skeleton.JointCount());
	for (auto & joint: skeleton) {
		skeletonPose.JointPoses.push_back(joint.BindPose);
	}

	return std::move(skeletonPose);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
