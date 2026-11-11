// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d::SkeletonHelper {

void traverse(
    const Skeleton& skeleton,
    const JointIndex& jointIndex,
    const std::function<void(Joint const&)>& traverser)
{
    POMDOG_ASSERT(jointIndex);
    auto& joint = skeleton.joints(jointIndex);
    traverser(joint);

    if (joint.firstChild) {
        traverse(skeleton, joint.firstChild, traverser);
    }
    if (joint.sibling) {
        traverse(skeleton, joint.sibling, traverser);
    }
}

void traverse(
    const Skeleton& skeleton,
    const std::function<void(Joint const&)>& traverser)
{
    POMDOG_ASSERT(skeleton.jointCount() > 0);
    POMDOG_ASSERT(skeleton.root().index);
    traverse(skeleton, skeleton.root().index, traverser);
}

void toGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose,
    std::vector<Matrix3x2>& globalPose)
{
    POMDOG_ASSERT(skeleton.jointCount() > 1);
    POMDOG_ASSERT(skeleton.root().index);

    SkeletonHelper::traverse(skeleton, skeleton.root().index, [&](const Joint& bone) {
        POMDOG_ASSERT(*bone.index < skeletonPose.jointPoses.size());
        auto& pose = skeletonPose.jointPoses[*bone.index];

        auto matrix = Matrix3x2::createScale(pose.scale);
        matrix *= Matrix3x2::createRotation(pose.rotation);
        matrix *= Matrix3x2::createTranslation(pose.translate);

        if (bone.parent) {
            POMDOG_ASSERT(*bone.parent < globalPose.size());
            matrix *= globalPose[*bone.parent];
        }

        POMDOG_ASSERT(*bone.index < globalPose.size());
        globalPose[*bone.index] = std::move(matrix);
    });
}

std::vector<Matrix3x2>
toGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose)
{
    std::vector<Matrix3x2> globalPose(skeleton.jointCount());
    SkeletonHelper::toGlobalPose(skeleton, skeletonPose, globalPose);
    return globalPose;
}

} // namespace pomdog::skeletal2d::SkeletonHelper
