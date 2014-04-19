//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Skeleton.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Skeleton::Skeleton(std::vector<Joint> && jointsIn)
	: joints(std::move(jointsIn))
{
	POMDOG_ASSERT(!joints.empty());
	POMDOG_ASSERT(!joints.front().Parent);
	
	bindPoses.reserve(joints.size());
	for (auto & localPose: joints)
	{
		bindPoses.push_back({
			localPose.Translate,
			localPose.Rotation,
			localPose.Scale
		});
	}
	
	globalPoses.resize(joints.size(), Matrix4x4::Identity);
}
//-----------------------------------------------------------------------
Joint const& Skeleton::Root() const
{
	POMDOG_ASSERT(!joints.empty());
	return joints.front();
}
//-----------------------------------------------------------------------
std::vector<Joint> const& Skeleton::Joints() const
{
	return joints;
}
//-----------------------------------------------------------------------
Joint const& Skeleton::Joints(JointIndex const& jointIndex) const
{
	POMDOG_ASSERT(jointIndex);
	POMDOG_ASSERT(*jointIndex < joints.size());
	return joints[*jointIndex];
}
//-----------------------------------------------------------------------
Joint & Skeleton::Joints(JointIndex const& jointIndex)
{
	POMDOG_ASSERT(jointIndex);
	POMDOG_ASSERT(*jointIndex < joints.size());
	return joints[*jointIndex];
}
//-----------------------------------------------------------------------
std::vector<JointPose> const& Skeleton::BindPoses() const
{
	return bindPoses;
}
//-----------------------------------------------------------------------
std::vector<Matrix4x4> const& Skeleton::GlobalPoses() const
{
	return globalPoses;
}
//-----------------------------------------------------------------------
std::vector<Matrix4x4> & Skeleton::GlobalPoses()
{
	return globalPoses;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
