// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_pose.hpp"
#include "pomdog/experimental/skeletal2d/skeleton.hpp"

namespace Pomdog::Skeletal2D {

SkeletonPose SkeletonPose::CreateBindPose(const Skeleton& skeleton)
{
    SkeletonPose skeletonPose;
    skeletonPose.JointPoses.reserve(skeleton.JointCount());
    for (auto& joint : skeleton) {
        skeletonPose.JointPoses.push_back(joint.BindPose);
    }

    return skeletonPose;
}

} // namespace Pomdog::Skeletal2D
