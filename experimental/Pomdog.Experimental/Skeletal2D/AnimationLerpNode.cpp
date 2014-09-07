//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationLerpNode.hpp"
#include "AnimationGraphWeightCollection.hpp"
#include "SkeletonPose.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include <algorithm>

namespace Pomdog {
namespace {

void Lerp(std::vector<JointPose> const& sourcePoses1, std::vector<JointPose> const& sourcePoses2,
	float weight, std::vector<JointPose> & output)
{
	POMDOG_ASSERT(!sourcePoses1.empty());
	POMDOG_ASSERT(!sourcePoses2.empty());
	POMDOG_ASSERT(sourcePoses1.size() == sourcePoses2.size());

	for (size_t i = 0; i < sourcePoses1.size(); ++i)
	{
		auto & pose1 = sourcePoses1[i];
		auto & pose2 = sourcePoses2[i];
		
		POMDOG_ASSERT(!output.empty());
		POMDOG_ASSERT(i < output.size());
		auto & result = output[i];

		result.Scale = MathHelper::Lerp(pose1.Scale, pose2.Scale, weight);
		result.Rotation = MathHelper::Lerp(pose1.Rotation.value, pose2.Rotation.value, weight);
		result.Translate = Vector2::Lerp(pose1.Translate, pose2.Translate, weight);
	}
}

}// unnamed namespace
//-----------------------------------------------------------------------
AnimationLerpNode::AnimationLerpNode(std::unique_ptr<AnimationNode> && blendNode1In,
	std::unique_ptr<AnimationNode> && blendNode2In, std::uint16_t weightIndexIn)
	: nodeA(std::move(blendNode1In))
	, nodeB(std::move(blendNode2In))
	, weightIndex(weightIndexIn)
{
	POMDOG_ASSERT(nodeA);
	POMDOG_ASSERT(nodeB);
	length = std::max(nodeA->Length(), nodeB->Length());
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationLerpNode::Length() const
{
	return length;
}
//-----------------------------------------------------------------------
std::unique_ptr<AnimationNode> const& AnimationLerpNode::A() const
{
	return nodeA;
}
//-----------------------------------------------------------------------
std::unique_ptr<AnimationNode> const& AnimationLerpNode::B() const
{
	return nodeB;
}
//-----------------------------------------------------------------------
void AnimationLerpNode::Calculate(AnimationTimeInterval const& time,
	AnimationGraphWeightCollection const& weights, Skeleton const& skeleton, SkeletonPose & skeletonPose) const
{
	auto sourcePose1 = SkeletonPose::CreateBindPose(skeleton);
	auto sourcePose2 = SkeletonPose::CreateBindPose(skeleton);

	POMDOG_ASSERT(nodeA);
	POMDOG_ASSERT(nodeB);
	
	nodeA->Calculate(time, weights, skeleton, sourcePose1);
	nodeB->Calculate(time, weights, skeleton, sourcePose2);
	
	auto weight = weights.At(weightIndex).GetFloat();
	Lerp(sourcePose1.JointPoses, sourcePose2.JointPoses, weight, skeletonPose.JointPoses);
}

}// namespace Pomdog
