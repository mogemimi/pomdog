//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP
#define POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Pomdog.hpp>
#include "Joint.hpp"

namespace Pomdog {

///@~Japanese
/// @note スケーリングが一様で Vector2 ではなく float が 1 つのみになっていることに注意してください。
/// スケーリングが一様なのは、パフォーマンス上の制約が理由になります。
class JointPose {
public:
	Vector2 Translate;
	Radian<float> Rotation;
	float Scale;
};

//class SkeletonPose {
//public:
//	Skeleton const Skeleton;
//	std::vector<JointPose> LocalPose;
//	std::vector<Matrix4x4> GlobalPose;
//	std::vector<JointPose> BindPose;
//};

///@~Japanese
/// @brief ジョイント（間接）またはボーンから構成されるスケルトンです。
class Skeleton {
public:
	Skeleton() = default;
	explicit Skeleton(std::vector<Joint> && joints);

	Skeleton(Skeleton const&) = delete;
	Skeleton & operator=(Skeleton const&) = delete;
	Skeleton(Skeleton &&) = default;
	Skeleton & operator=(Skeleton &&) = default;

	Joint const& Root() const;
	std::vector<Joint> const& Joints() const;
	Joint const& Joints(JointIndex const& jointIndex) const;
	Joint & Joints(JointIndex const& jointIndex);
	
	std::vector<JointPose> const& BindPoses() const;
	std::vector<Matrix4x4> const& GlobalPoses() const;
	std::vector<Matrix4x4> & GlobalPoses();

private:
	std::vector<Joint> joints;
	//std::vector<JointPose> localPoses;
	std::vector<Matrix4x4> globalPoses;
	std::vector<JointPose> bindPoses;
};

}// namespace Pomdog

#endif // !defined(POMDOG_SKELETON_015B5D6C_C3A5_4AC7_B9BB_501EF76F5C7B_HPP)
