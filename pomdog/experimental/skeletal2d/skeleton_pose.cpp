// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"

namespace pomdog::skeletal2d {

SkeletonPose SkeletonPose::CreateBindPose(const Skeleton& skeleton)
{
    SkeletonPose skeletonPose;
    skeletonPose.JointPoses.reserve(skeleton.JointCount());
    for (auto& joint : skeleton) {
        skeletonPose.JointPoses.push_back(joint.BindPose);
    }

    return skeletonPose;
}

} // namespace pomdog::skeletal2d
