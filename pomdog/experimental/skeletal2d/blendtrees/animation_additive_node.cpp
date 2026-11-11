// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_additive_node.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/skeleton_pose.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/math/vector2.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

SkeletonPose createEmptyPose(const Skeleton& skeleton)
{
    SkeletonPose skeletonPose;
    JointPose jointPose;
    jointPose.scale = 0;
    jointPose.rotation = 0;
    jointPose.translate = Vector2::createZero();
    skeletonPose.jointPoses.resize(skeleton.jointCount(), jointPose);
    return skeletonPose;
}

} // namespace

AnimationAdditiveNode::AnimationAdditiveNode(
    std::unique_ptr<AnimationNode>&& blendNode1In,
    std::unique_ptr<AnimationNode>&& blendNode2In)
    : base_(std::move(blendNode1In))
    , additive_(std::move(blendNode2In))
    , weight_(1.0f)
{
    POMDOG_ASSERT(base_);
    POMDOG_ASSERT(additive_);
    length_ = std::max(base_->getLength(), additive_->getLength());
}

f32 AnimationAdditiveNode::getWeight() const noexcept
{
    return this->weight_;
}

void AnimationAdditiveNode::setWeight(f32 weightIn) noexcept
{
    this->weight_ = weightIn;
}

AnimationTimeInterval AnimationAdditiveNode::getLength() const
{
    return length_;
}

void AnimationAdditiveNode::calculate(
    const AnimationTimeInterval& time,
    const AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::createBindPose(skeleton);
    // auto sourcePose2 = SkeletonPose::createBindPose(skeleton);
    auto sourcePose2 = createEmptyPose(skeleton);

    POMDOG_ASSERT(base_);
    POMDOG_ASSERT(additive_);

    base_->calculate(time, weights, skeleton, sourcePose1, skin);
    additive_->calculate(time, weights, skeleton, sourcePose2, nullptr);

    // SkeletonPose bindPose = SkeletonHelper::createBindPose(skeleton);
    //
    // for (size_t i = 0; i < sourcePose2.jointPoses.size(); ++i) {
    //     auto & pose = bindPose.jointPoses[i];
    //     auto & additivePose = sourcePose2.jointPoses[i];
    //
    //     additivePose.scale = additivePose.scale - pose.scale;
    //     additivePose.rotation = additivePose.rotation - pose.rotation;
    //     additivePose.translate = additivePose.translate - pose.translate;
    // }

    POMDOG_ASSERT(!sourcePose1.jointPoses.empty());
    POMDOG_ASSERT(!sourcePose2.jointPoses.empty());
    POMDOG_ASSERT(sourcePose1.jointPoses.size() == sourcePose2.jointPoses.size());

    for (std::size_t i = 0; i < sourcePose1.jointPoses.size(); ++i) {
        auto& basePose = sourcePose1.jointPoses[i];
        auto& additivePose = sourcePose2.jointPoses[i];

        POMDOG_ASSERT(i < skeletonPose.jointPoses.size());
        auto& result = skeletonPose.jointPoses[i];

        result.scale = basePose.scale + (additivePose.scale * weight_);
        result.rotation = basePose.rotation + (additivePose.rotation * weight_);
        result.translate = basePose.translate + (additivePose.translate * weight_);
    }
}

} // namespace pomdog::skeletal2d::detail
