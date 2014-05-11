//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkeletonHelper.hpp"
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Math/Matrix4x4.hpp>
#include "JointIndex.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
void SkeletonHelper::Traverse(Skeleton const& skeleton, JointIndex const& jointIndex,
	std::function<void(Joint const&)> const& traverser)
{
	POMDOG_ASSERT(jointIndex);
	auto & joint = skeleton.Joints(jointIndex);
	traverser(joint);
	
	if (joint.FirstChild) {
		Traverse(skeleton, joint.FirstChild, traverser);
	}
	if (joint.Sibling) {
		Traverse(skeleton, joint.Sibling, traverser);
	}
}
//-----------------------------------------------------------------------
void SkeletonHelper::Traverse(Skeleton const& skeleton,
	std::function<void(Joint const&)> const& traverser)
{
	POMDOG_ASSERT(skeleton.JointCount() > 0);
	POMDOG_ASSERT(skeleton.Root().Index);
	Traverse(skeleton, skeleton.Root().Index, traverser);
}
//-----------------------------------------------------------------------
void SkeletonHelper::ComputeGlobalPoseFromLocalPose(
	Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	{
		auto bone = skeleton.Root();
		POMDOG_ASSERT(*bone.Index < skeletonPose.LocalPose.size());
		auto & pose = skeletonPose.LocalPose[*bone.Index];
	
		Matrix4x4 matrix = Matrix4x4::CreateScale(pose.Scale);
		matrix *= Matrix4x4::CreateRotationZ(pose.Rotation);
		matrix *= Matrix4x4::CreateTranslation({pose.Translate, 0.0f});
	
		POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
		skeletonPose.GlobalPose[*bone.Index] = Matrix4x4::Identity;
	}

	POMDOG_ASSERT(skeleton.JointCount() > 1);
	POMDOG_ASSERT(skeleton.Root().Index);
	POMDOG_ASSERT(skeleton.Root().FirstChild);

	SkeletonHelper::Traverse(skeleton, skeleton.Root().FirstChild, [&](Joint const& bone)
	{
		POMDOG_ASSERT(*bone.Index < skeletonPose.LocalPose.size());
		auto & pose = skeletonPose.LocalPose[*bone.Index];
		
		POMDOG_ASSERT(bone.Parent);
		POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
		auto & parentMatrix = skeletonPose.GlobalPose[*bone.Parent];

		Matrix4x4 matrix = Matrix4x4::CreateScale(pose.Scale);
		matrix *= Matrix4x4::CreateRotationZ(pose.Rotation);
		matrix *= Matrix4x4::CreateTranslation({pose.Translate, 0.0f});
		matrix *= parentMatrix;

		POMDOG_ASSERT(*bone.Index < skeletonPose.GlobalPose.size());
		skeletonPose.GlobalPose[*bone.Index] = matrix;
	});
}
//-----------------------------------------------------------------------
}// namespace Pomdog
