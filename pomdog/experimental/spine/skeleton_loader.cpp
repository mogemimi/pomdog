// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skeleton_loader.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/joint.hpp"
#include "pomdog/experimental/skeletal2d/joint_index.hpp"
#include "pomdog/experimental/skeletal2d/skeleton.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_helper.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_pose.hpp"
#include "pomdog/experimental/spine/skeleton_desc.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::Joint;
using skeletal2d::JointIndex;
using skeletal2d::Skeleton;

JointIndex
FindBoneParent(
    const std::string& parentName,
    [[maybe_unused]] const std::vector<Joint>& bones,
    const std::vector<BoneDesc>& boneDescs)
{
    auto iter = std::find_if(std::begin(boneDescs), std::end(boneDescs),
        [&parentName](const BoneDesc& desc) { return desc.Name == parentName; });

    if (iter == std::end(boneDescs)) {
        return {};
    }

    std::size_t jointIndex = std::distance(std::begin(boneDescs), iter);
    POMDOG_ASSERT(jointIndex < bones.size());
    POMDOG_ASSERT(jointIndex < std::numeric_limits<typename std::remove_reference<
        decltype(*JointIndex())>::type>::max());
    return JointIndex{static_cast<std::uint8_t>(jointIndex)};
}

std::vector<Matrix3x2> CreateInverseBindPoseByJoints(const Skeleton& skeleton)
{
    POMDOG_ASSERT(skeleton.JointCount() > 0);
    POMDOG_ASSERT(skeleton.Root().Index);

    std::vector<Matrix3x2> bindPose;
    bindPose.resize(skeleton.JointCount());

    skeletal2d::SkeletonHelper::Traverse(skeleton, skeleton.Root().Index, [&bindPose](const Joint& bone) {
        Matrix3x2 matrix = Matrix3x2::CreateScale(bone.BindPose.Scale);
        matrix *= Matrix3x2::CreateRotation(bone.BindPose.Rotation);
        matrix *= Matrix3x2::CreateTranslation(bone.BindPose.Translate);

        if (bone.Parent) {
            POMDOG_ASSERT(*bone.Parent < bindPose.size());
            auto& parentMatrix = bindPose[*bone.Parent];
            matrix *= parentMatrix;
        }

        POMDOG_ASSERT(*bone.Index < bindPose.size());
        bindPose[*bone.Index] = matrix;
    });

    for (auto& matrix : bindPose) {
        matrix = Matrix3x2::Invert(matrix);
    }
    return bindPose;
}

std::vector<Joint> CreateBones(const std::vector<BoneDesc>& boneDescriptions)
{
    std::vector<Joint> joints;
    joints.reserve(boneDescriptions.size());

    for (auto& boneDesc : boneDescriptions) {
        Joint joint;
        joint.BindPose.Translate = boneDesc.Pose.Translate;
        joint.BindPose.Rotation = boneDesc.Pose.Rotation;
        joint.BindPose.Scale = boneDesc.Pose.Scale;

        joints.push_back(std::move(joint));
    }

    POMDOG_ASSERT(joints.size() == boneDescriptions.size());

    for (std::uint32_t index = 0; index < joints.size(); ++index) {
        auto& joint = joints[index];
        joint.Index = static_cast<std::uint8_t>(index);

        auto& boneDesc = boneDescriptions[index];

        POMDOG_ASSERT(boneDesc.Name != boneDesc.Parent);
        if (boneDesc.Name == boneDesc.Parent) {
            // Error
            continue;
        }
        joint.Parent = FindBoneParent(boneDesc.Parent, joints, boneDescriptions);
    }

    POMDOG_ASSERT(!joints.front().Parent);

    for (auto& joint : joints) {
        if (!joint.Parent) {
            continue;
        }

        if (!joints[*joint.Parent].FirstChild) {
            joints[*joint.Parent].FirstChild = joint.Index;
        }
        else {
            POMDOG_ASSERT(joints[*joint.Parent].FirstChild);
            auto sibling = joints[*joint.Parent].FirstChild;

            POMDOG_ASSERT(sibling);
            POMDOG_ASSERT(sibling != joint.Index);

            do {
                if (!joints[*sibling].Sibling) {
                    joints[*sibling].Sibling = joint.Index;
                    break;
                }
                sibling = joints[*sibling].Sibling;
            } while (sibling);
        }
    }

    return joints;
}

} // namespace

skeletal2d::Skeleton CreateSkeleton(const std::vector<spine::BoneDesc>& bones)
{
    POMDOG_ASSERT(!bones.empty());

    skeletal2d::Skeleton skeleton{CreateBones(bones)};

    auto inverseBindPose = CreateInverseBindPoseByJoints(skeleton);
    for (auto& joint : skeleton) {
        POMDOG_ASSERT(joint.Index);
        joint.InverseBindPose = inverseBindPose[*joint.Index];
    }

    return skeleton;
}

} // namespace pomdog::spine
