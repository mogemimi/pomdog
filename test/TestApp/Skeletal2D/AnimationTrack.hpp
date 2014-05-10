//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP
#define POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Application/DurationSeconds.hpp>

namespace Pomdog {

class Skeleton;
class SkeletonPose;

class AnimationTrack {
public:
	virtual ~AnimationTrack() = default;
	
	virtual void Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose) = 0;
	
	virtual DurationSeconds Length() const = 0;
};


//template <typename T>
//class AnimationKey {
//public:
//	float TimeSeconds;
//};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTRACK_70C41AD9_88F5_4283_B0A7_08CDE74CCDDC_HPP)
