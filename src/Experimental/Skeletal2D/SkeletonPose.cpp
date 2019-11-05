// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog/Experimental/Skeletal2D/Skeleton.hpp"

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
