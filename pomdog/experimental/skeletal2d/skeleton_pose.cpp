// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"

namespace pomdog::skeletal2d {

SkeletonPose SkeletonPose::createBindPose(const Skeleton& skeleton)
{
    SkeletonPose skeletonPose;
    skeletonPose.jointPoses.reserve(skeleton.jointCount());
    for (auto& joint : skeleton) {
        skeletonPose.jointPoses.push_back(joint.bindPose);
    }

    return skeletonPose;
}

} // namespace pomdog::skeletal2d
