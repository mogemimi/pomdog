//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SCALETRACK_D087D656_A9AE_450F_A413_CC4BB795C36C_HPP
#define POMDOG_SCALETRACK_D087D656_A9AE_450F_A413_CC4BB795C36C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Application/DurationSeconds.hpp>
#include "AnimationTrack.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"

namespace Pomdog {
namespace Details {
namespace Skeletal2D {

class ScaleKeyframe {
public:
	float TimeSeconds;
	CompressedFloat<std::int16_t, 1500> Scale;

	static_assert(decltype(Scale)::Max() > 20.0f, "");
	static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class ScaleTrack final: public AnimationTrack {
public:
	ScaleTrack(std::vector<ScaleKeyframe> && keys, JointIndex && jointIndex);

	void Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose) override;
	
	DurationSeconds Length() const override;
	
private:
	std::vector<ScaleKeyframe> keys;
	JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SCALETRACK_D087D656_A9AE_450F_A413_CC4BB795C36C_HPP)
