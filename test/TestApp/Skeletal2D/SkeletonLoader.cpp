//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SkeletonLoader.hpp"
#include <utility>
#include <limits>
#include "SkeletonDesc.hpp"
#include "JointIndex.hpp"
#include "Joint.hpp"
#include "Skeleton.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
//-----------------------------------------------------------------------
namespace {

static auto FindBoneParent(std::string const& parentName,
	std::vector<Joint> const& bones, std::vector<BoneDesc> const& boneDescs)-> decltype(Joint::Index)
{
	auto iter = std::find_if(std::begin(boneDescs), std::end(boneDescs), [&parentName](Skeletal2D::BoneDesc const& desc) {
		return desc.Name == parentName;
	});

	if (iter == std::end(boneDescs)) {
		return {};
	}

	std::size_t jointIndex = std::distance(std::begin(boneDescs), iter);
	POMDOG_ASSERT(jointIndex < bones.size());
	POMDOG_ASSERT(jointIndex < std::numeric_limits<typename std::remove_reference<decltype(*decltype(Joint::Index)())>::type>::max());
	return jointIndex;
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
Skeleton CreateSkeleton(std::vector<Skeletal2D::BoneDesc> const& bones)
{
	POMDOG_ASSERT(!bones.empty());
	
	Skeleton skeleton(CreateBones(bones));
	return std::move(skeleton);
}

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
