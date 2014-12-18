//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ROTATIONTRACK_96A9D69A_686F_41DF_B6CB_8AB551DD12D5_HPP
#define POMDOG_ROTATIONTRACK_96A9D69A_686F_41DF_B6CB_8AB551DD12D5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include <vector>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class RotationKeyframe {
public:
	AnimationTimeInterval Time;
	CompressedFloat<std::int16_t, 4500> Rotation;
		
	static_assert(decltype(Rotation)::Max() > 3.1416f*2.0f, "");
	static_assert(decltype(Rotation)::Min() < -3.1416f*2.0f, "");
};

class RotationTrack final: public AnimationTrack {
public:
	RotationTrack(std::vector<RotationKeyframe> && keys, JointIndex && jointIndex);

	void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
		SkeletonPose & skeletonPose) override;
	
	AnimationTimeInterval Length() const override;
	
private:
	std::vector<RotationKeyframe> keys;
	JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ROTATIONTRACK_96A9D69A_686F_41DF_B6CB_8AB551DD12D5_HPP)
