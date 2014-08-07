//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkeletonLoader.hpp"
#include "Pomdog.Experimental/Spine/SkeletonDesc.hpp"
#include "Pomdog.Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog.Experimental/Skeletal2D/Joint.hpp"
#include "Pomdog.Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonPose.hpp"
#include "Pomdog.Experimental/Skeletal2D/SkeletonHelper.hpp"
#include <utility>
#include <limits>

namespace Pomdog {
namespace Details {
namespace Spine {
//-----------------------------------------------------------------------
namespace {

static auto FindBoneParent(std::string const& parentName,
	std::vector<Joint> const& bones, std::vector<BoneDesc> const& boneDescs)-> decltype(Joint::Index)
{
	auto iter = std::find_if(std::begin(boneDescs), std::end(boneDescs),
		[&parentName](BoneDesc const& desc) { return desc.Name == parentName; });

	if (iter == std::end(boneDescs)) {
		return {};
	}

	std::size_t jointIndex = std::distance(std::begin(boneDescs), iter);
	POMDOG_ASSERT(jointIndex < bones.size());
	POMDOG_ASSERT(jointIndex < std::numeric_limits<typename std::remove_reference<
		decltype(*decltype(Joint::Index)())>::type>::max());
	return jointIndex;
}
//-----------------------------------------------------------------------
static std::vector<Matrix3x2> CreateInverseBindPoseByJoints(Skeleton const& skeleton)
{
	POMDOG_ASSERT(skeleton.JointCount() > 0);
	POMDOG_ASSERT(skeleton.Root().Index);

	std::vector<Matrix3x2> bindPose;
	bindPose.resize(skeleton.JointCount());

	SkeletonHelper::Traverse(skeleton, skeleton.Root().Index, [&bindPose](Joint const& bone)
	{
		Matrix3x2 matrix = Matrix3x2::CreateScale(bone.BindPose.Scale);
		matrix *= Matrix3x2::CreateRotation(bone.BindPose.Rotation);
		matrix *= Matrix3x2::CreateTranslation(bone.BindPose.Translate);
		
		if (bone.Parent)
		{
			POMDOG_ASSERT(*bone.Parent < bindPose.size());
			auto & parentMatrix = bindPose[*bone.Parent];
			matrix *= parentMatrix;
		}

		POMDOG_ASSERT(*bone.Index < bindPose.size());
		bindPose[*bone.Index] = matrix;
	});
	
	for (auto & matrix: bindPose)
	{
		matrix = Matrix3x2::Invert(matrix);
	}
	return std::move(bindPose);
}
//-----------------------------------------------------------------------
static std::vector<Joint> CreateBones(std::vector<BoneDesc> const& boneDescriptions)
{
	std::vector<Joint> joints;
	joints.reserve(boneDescriptions.size());

	for (auto & boneDesc: boneDescriptions)
	{
		Joint joint;
		joint.BindPose.Translate = boneDesc.Pose.Translate;
		joint.BindPose.Rotation = boneDesc.Pose.Rotation;
		joint.BindPose.Scale = boneDesc.Pose.Scale;
		
		joints.push_back(std::move(joint));
	}
	
	POMDOG_ASSERT(joints.size() == boneDescriptions.size());

	for (std::uint32_t index = 0; index < joints.size(); ++index)
	{
		auto & joint = joints[index];
		joint.Index = index;
		
		auto & boneDesc = boneDescriptions[index];
		
		POMDOG_ASSERT(boneDesc.Name != boneDesc.Parent);
		if (boneDesc.Name == boneDesc.Parent) {
			// Error
			continue;
		}
		joint.Parent = FindBoneParent(boneDesc.Parent, joints, boneDescriptions);
	}
	
	POMDOG_ASSERT(!joints.front().Parent);
	
	for (auto & joint: joints)
	{
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

	return std::move(joints);
}

}// unnamed namespace
//-----------------------------------------------------------------------
Skeleton CreateSkeleton(std::vector<Spine::BoneDesc> const& bones)
{
	POMDOG_ASSERT(!bones.empty());
	
	Skeleton skeleton(CreateBones(bones));

	auto inverseBindPose = CreateInverseBindPoseByJoints(skeleton);
	for (auto & joint: skeleton)
	{
		POMDOG_ASSERT(joint.Index);
		joint.InverseBindPose = inverseBindPose[*joint.Index];
	}
	
	return std::move(skeleton);
}

}// namespace Spine
}// namespace Details
}// namespace Pomdog
