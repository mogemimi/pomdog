//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SkeletonHelper.hpp"
#include "JointIndex.hpp"
#include "Skeleton.hpp"
#include "SkeletonPose.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"

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
void SkeletonHelper::ToGlobalPose(Skeleton const& skeleton,
	SkeletonPose const& skeletonPose, std::vector<Matrix3x2> & globalPose)
{
	POMDOG_ASSERT(skeleton.JointCount() > 1);
	POMDOG_ASSERT(skeleton.Root().Index);

	SkeletonHelper::Traverse(skeleton, skeleton.Root().Index, [&](Joint const& bone)
	{
		POMDOG_ASSERT(*bone.Index < skeletonPose.JointPoses.size());
		auto & pose = skeletonPose.JointPoses[*bone.Index];

		Matrix3x2 matrix = Matrix3x2::CreateScale(pose.Scale);
		matrix *= Matrix3x2::CreateRotation(pose.Rotation);
		matrix *= Matrix3x2::CreateTranslation(pose.Translate);
		
		if (bone.Parent)
		{
			POMDOG_ASSERT(*bone.Parent < globalPose.size());
			matrix *= globalPose[*bone.Parent];
		}

		POMDOG_ASSERT(*bone.Index < globalPose.size());
		globalPose[*bone.Index] = std::move(matrix);
	});
}
//-----------------------------------------------------------------------
std::vector<Matrix3x2> SkeletonHelper::ToGlobalPose(Skeleton const& skeleton,
	SkeletonPose const& skeletonPose)
{
	std::vector<Matrix3x2> globalPose(skeleton.JointCount());
	SkeletonHelper::ToGlobalPose(skeleton, skeletonPose, globalPose);
	return std::move(globalPose);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
