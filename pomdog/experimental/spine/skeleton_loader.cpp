// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skeleton_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/joint.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/experimental/spine/skeleton_data.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skeleton_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

using skeletal2d::Joint;
using skeletal2d::JointIndex;
using skeletal2d::Skeleton;

// NOTE: The file marks a joint without a parent with 255, which is also the
// value of an empty JointIndex.
static_assert(!JointIndex{u8(255)});

std::vector<Matrix3x2> CreateInverseBindPoseByJoints(const Skeleton& skeleton)
{
    POMDOG_ASSERT(skeleton.JointCount() > 0);
    POMDOG_ASSERT(skeleton.Root().Index);

    std::vector<Matrix3x2> bindPose;
    bindPose.resize(skeleton.JointCount());

    skeletal2d::SkeletonHelper::Traverse(skeleton, skeleton.Root().Index, [&bindPose](const Joint& bone) {
        auto matrix = Matrix3x2::createScale(bone.BindPose.Scale);
        matrix *= Matrix3x2::createRotation(bone.BindPose.Rotation);
        matrix *= Matrix3x2::createTranslation(bone.BindPose.Translate);

        if (bone.Parent) {
            POMDOG_ASSERT(*bone.Parent < bindPose.size());
            auto& parentMatrix = bindPose[*bone.Parent];
            matrix *= parentMatrix;
        }

        POMDOG_ASSERT(*bone.Index < bindPose.size());
        bindPose[*bone.Index] = matrix;
    });

    for (auto& matrix : bindPose) {
        matrix = math::invert(matrix);
    }
    return bindPose;
}

std::vector<Joint> CreateJoints(const pomdogschemas::Skeletal2DSkeleton& table)
{
    const auto* source = table.joints();
    POMDOG_ASSERT(source != nullptr);

    std::vector<Joint> joints;
    joints.reserve(source->size());

    for (const auto* jointData : *source) {
        Joint joint;
        joint.BindPose.Translate = Vector2{jointData->translate_x(), jointData->translate_y()};
        joint.BindPose.Rotation = jointData->rotation();
        joint.BindPose.Scale = jointData->scale();
        joint.Index = static_cast<u8>(joints.size());
        joint.Parent = JointIndex{jointData->parent()};

        joints.push_back(std::move(joint));
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

skeletal2d::Skeleton CreateSkeleton(const SkeletonData& skeletonData)
{
    skeletal2d::Skeleton skeleton{CreateJoints(skeletonData.getTable())};

    auto inverseBindPose = CreateInverseBindPoseByJoints(skeleton);
    for (auto& joint : skeleton) {
        POMDOG_ASSERT(joint.Index);
        joint.InverseBindPose = inverseBindPose[*joint.Index];
    }

    return skeleton;
}

} // namespace pomdog::spine
