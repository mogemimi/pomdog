//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP
#define POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "AnimationTimeInterval.hpp"

namespace Pomdog {

class Skeleton;
class SkeletonPose;

class AnimationTrack {
public:
	virtual ~AnimationTrack() = default;

	virtual void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
		SkeletonPose & skeletonPose) = 0;

	virtual AnimationTimeInterval Length() const = 0;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP)
