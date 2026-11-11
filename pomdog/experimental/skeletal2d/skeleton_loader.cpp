// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_loader.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/joint.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_data.h"
#include "pomdog/experimental/skeletal2d/skeleton_helper.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skeleton_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

// NOTE: The file marks a joint without a parent with 255, which is also the
// value of an empty JointIndex.
static_assert(!JointIndex{u8(255)});

std::vector<Matrix3x2> createInverseBindPoseByJoints(const Skeleton& skeleton)
{
    POMDOG_ASSERT(skeleton.jointCount() > 0);
    POMDOG_ASSERT(skeleton.root().index);

    std::vector<Matrix3x2> bindPose;
    bindPose.resize(skeleton.jointCount());

    SkeletonHelper::traverse(skeleton, skeleton.root().index, [&bindPose](const Joint& bone) {
        auto matrix = Matrix3x2::createScale(bone.bindPose.scale);
        matrix *= Matrix3x2::createRotation(bone.bindPose.rotation);
        matrix *= Matrix3x2::createTranslation(bone.bindPose.translate);

        if (bone.parent) {
            POMDOG_ASSERT(*bone.parent < bindPose.size());
            auto& parentMatrix = bindPose[*bone.parent];
            matrix *= parentMatrix;
        }

        POMDOG_ASSERT(*bone.index < bindPose.size());
        bindPose[*bone.index] = matrix;
    });

    for (auto& matrix : bindPose) {
        matrix = math::invert(matrix);
    }
    return bindPose;
}

std::vector<Joint> createJoints(const pomdogschemas::Skeletal2DSkeleton& table)
{
    const auto* source = table.joints();
    POMDOG_ASSERT(source != nullptr);

    std::vector<Joint> joints;
    joints.reserve(source->size());

    for (const auto* jointData : *source) {
        Joint joint;
        joint.bindPose.translate = Vector2{jointData->translate_x(), jointData->translate_y()};
        joint.bindPose.rotation = jointData->rotation();
        joint.bindPose.scale = jointData->scale();
        joint.index = static_cast<u8>(joints.size());
        joint.parent = JointIndex{jointData->parent()};

        joints.push_back(std::move(joint));
    }

    POMDOG_ASSERT(!joints.front().parent);

    for (auto& joint : joints) {
        if (!joint.parent) {
            continue;
        }

        if (!joints[*joint.parent].firstChild) {
            joints[*joint.parent].firstChild = joint.index;
        }
        else {
            POMDOG_ASSERT(joints[*joint.parent].firstChild);
            auto sibling = joints[*joint.parent].firstChild;

            POMDOG_ASSERT(sibling);
            POMDOG_ASSERT(sibling != joint.index);

            do {
                if (!joints[*sibling].sibling) {
                    joints[*sibling].sibling = joint.index;
                    break;
                }
                sibling = joints[*sibling].sibling;
            } while (sibling);
        }
    }

    return joints;
}

} // namespace

Skeleton createSkeleton(const SkeletonData& skeletonData)
{
    Skeleton skeleton{createJoints(skeletonData.getTable())};

    auto inverseBindPose = createInverseBindPoseByJoints(skeleton);
    for (auto& joint : skeleton) {
        POMDOG_ASSERT(joint.index);
        joint.inverseBindPose = inverseBindPose[*joint.index];
    }

    return skeleton;
}

} // namespace pomdog::skeletal2d
