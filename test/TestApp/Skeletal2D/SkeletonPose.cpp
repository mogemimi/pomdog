//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
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
