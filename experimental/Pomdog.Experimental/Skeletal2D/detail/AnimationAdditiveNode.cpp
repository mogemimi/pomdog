// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "AnimationAdditiveNode.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/MathHelper.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {
namespace {

static SkeletonPose CreateEmptyPose(Skeleton const& skeleton)
{
    SkeletonPose skeletonPose;
    JointPose jointPose;
    jointPose.Scale = 0;
    jointPose.Rotation = 0;
    jointPose.Translate = Vector2::Zero;
    skeletonPose.JointPoses.resize(skeleton.JointCount(), jointPose);
    return std::move(skeletonPose);
}

}// unnamed namespace
//-----------------------------------------------------------------------
AnimationAdditiveNode::AnimationAdditiveNode(std::unique_ptr<AnimationNode> && blendNode1In,
    std::unique_ptr<AnimationNode> && blendNode2In)
    : base(std::move(blendNode1In))
    , additive(std::move(blendNode2In))
    , weight(1.0f)
{
    POMDOG_ASSERT(base);
    POMDOG_ASSERT(additive);
    length = std::max(base->Length(), additive->Length());
}
//-----------------------------------------------------------------------
float AnimationAdditiveNode::Weight() const
{
    return this->weight;
}
//-----------------------------------------------------------------------
void AnimationAdditiveNode::Weight(float weightIn)
{
    this->weight = weightIn;
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationAdditiveNode::Length() const
{
    return length;
}
//-----------------------------------------------------------------------
void AnimationAdditiveNode::Calculate(AnimationTimeInterval const& time,
    Detail::Skeletal2D::AnimationGraphWeightCollection const& weights,
    Skeleton const& skeleton, SkeletonPose & skeletonPose) const
{
    auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
//    auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);
    auto sourcePose2 = CreateEmptyPose(skeleton);

    POMDOG_ASSERT(base);
    POMDOG_ASSERT(additive);

    base->Calculate(time, weights, skeleton, sourcePose1);
    additive->Calculate(time, weights, skeleton, sourcePose2);

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

    for (size_t i = 0; i < sourcePose1.JointPoses.size(); ++i)
    {
        auto & basePose = sourcePose1.JointPoses[i];
        auto & additivePose = sourcePose2.JointPoses[i];

        POMDOG_ASSERT(i < skeletonPose.JointPoses.size());
        auto & result = skeletonPose.JointPoses[i];

        result.Scale = basePose.Scale + (additivePose.Scale * weight);
        result.Rotation = basePose.Rotation + (additivePose.Rotation * weight);
        result.Translate = basePose.Translate + (additivePose.Translate * weight);
    }
}

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog
