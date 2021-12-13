// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/animation_additive_node.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/skeleton.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_helper.hpp"
#include "pomdog/experimental/skeletal2d/skeleton_pose.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/math/vector2.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

SkeletonPose CreateEmptyPose(const Skeleton& skeleton)
{
    SkeletonPose skeletonPose;
    JointPose jointPose;
    jointPose.Scale = 0;
    jointPose.Rotation = 0;
    jointPose.Translate = Vector2::Zero();
    skeletonPose.JointPoses.resize(skeleton.JointCount(), jointPose);
    return skeletonPose;
}

} // namespace

AnimationAdditiveNode::AnimationAdditiveNode(
    std::unique_ptr<AnimationNode>&& blendNode1In,
    std::unique_ptr<AnimationNode>&& blendNode2In)
    : base(std::move(blendNode1In))
    , additive(std::move(blendNode2In))
    , weight(1.0f)
{
    POMDOG_ASSERT(base);
    POMDOG_ASSERT(additive);
    length = std::max(base->GetLength(), additive->GetLength());
}

float AnimationAdditiveNode::GetWeight() const noexcept
{
    return this->weight;
}

void AnimationAdditiveNode::SetWeight(float weightIn) noexcept
{
    this->weight = weightIn;
}

AnimationTimeInterval AnimationAdditiveNode::GetLength() const
{
    return length;
}

void AnimationAdditiveNode::Calculate(
    const AnimationTimeInterval& time,
    const AnimationGraphWeightCollection& weights,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin) const
{
    auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
//    auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);
    auto sourcePose2 = CreateEmptyPose(skeleton);

    POMDOG_ASSERT(base);
    POMDOG_ASSERT(additive);

    base->Calculate(time, weights, skeleton, sourcePose1, skin);
    additive->Calculate(time, weights, skeleton, sourcePose2, nullptr);

//    SkeletonPose bindPose = SkeletonHelper::CreateBindPose(skeleton);
//
//    for (size_t i = 0; i < sourcePose2.JointPoses.size(); ++i)
//    {
//        auto & pose = bindPose.JointPoses[i];
//        auto & additivePose = sourcePose2.JointPoses[i];
//
//        additivePose.Scale = additivePose.Scale - pose.Scale;
//        additivePose.Rotation = additivePose.Rotation - pose.Rotation;
//        additivePose.Translate = additivePose.Translate - pose.Translate;
//    }

    POMDOG_ASSERT(!sourcePose1.JointPoses.empty());
    POMDOG_ASSERT(!sourcePose2.JointPoses.empty());
    POMDOG_ASSERT(sourcePose1.JointPoses.size() == sourcePose2.JointPoses.size());

    for (std::size_t i = 0; i < sourcePose1.JointPoses.size(); ++i) {
        auto& basePose = sourcePose1.JointPoses[i];
        auto& additivePose = sourcePose2.JointPoses[i];

        POMDOG_ASSERT(i < skeletonPose.JointPoses.size());
        auto& result = skeletonPose.JointPoses[i];

        result.Scale = basePose.Scale + (additivePose.Scale * weight);
        result.Rotation = basePose.Rotation + (additivePose.Rotation * weight);
        result.Translate = basePose.Translate + (additivePose.Translate * weight);
    }
}

} // namespace pomdog::skeletal2d::detail
