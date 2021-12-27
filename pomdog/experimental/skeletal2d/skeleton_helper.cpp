// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d::SkeletonHelper {

void Traverse(
    const Skeleton& skeleton,
    const JointIndex& jointIndex,
    const std::function<void(Joint const&)>& traverser)
{
    POMDOG_ASSERT(jointIndex);
    auto& joint = skeleton.Joints(jointIndex);
    traverser(joint);

    if (joint.FirstChild) {
        Traverse(skeleton, joint.FirstChild, traverser);
    }
    if (joint.Sibling) {
        Traverse(skeleton, joint.Sibling, traverser);
    }
}

void Traverse(
    const Skeleton& skeleton,
    const std::function<void(Joint const&)>& traverser)
{
    POMDOG_ASSERT(skeleton.JointCount() > 0);
    POMDOG_ASSERT(skeleton.Root().Index);
    Traverse(skeleton, skeleton.Root().Index, traverser);
}

void ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose,
    std::vector<Matrix3x2>& globalPose)
{
    POMDOG_ASSERT(skeleton.JointCount() > 1);
    POMDOG_ASSERT(skeleton.Root().Index);

    SkeletonHelper::Traverse(skeleton, skeleton.Root().Index, [&](const Joint& bone) {
        POMDOG_ASSERT(*bone.Index < skeletonPose.JointPoses.size());
        auto& pose = skeletonPose.JointPoses[*bone.Index];

        Matrix3x2 matrix = Matrix3x2::CreateScale(pose.Scale);
        matrix *= Matrix3x2::CreateRotation(pose.Rotation);
        matrix *= Matrix3x2::CreateTranslation(pose.Translate);

        if (bone.Parent) {
            POMDOG_ASSERT(*bone.Parent < globalPose.size());
            matrix *= globalPose[*bone.Parent];
        }

        POMDOG_ASSERT(*bone.Index < globalPose.size());
        globalPose[*bone.Index] = std::move(matrix);
    });
}

std::vector<Matrix3x2>
ToGlobalPose(
    const Skeleton& skeleton,
    const SkeletonPose& skeletonPose)
{
    std::vector<Matrix3x2> globalPose(skeleton.JointCount());
    SkeletonHelper::ToGlobalPose(skeleton, skeletonPose, globalPose);
    return globalPose;
}

} // namespace pomdog::skeletal2d::SkeletonHelper
