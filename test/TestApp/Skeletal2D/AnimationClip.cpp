//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AnimationClip.hpp"
#include <utility>
#include "Skeleton.hpp"
#include "AnimationTimeline.hpp"

namespace Pomdog {

AnimationClip::AnimationClip(std::vector<std::unique_ptr<AnimationTimeline>> && timelinesIn)
	: timelines(std::move(timelinesIn))
{}

void AnimationClip::Apply(Skeleton & skeleton, DurationSeconds const& time)
{
	for (auto & timeline: timelines)
	{
		POMDOG_ASSERT(timeline);
		timeline->Apply(skeleton, time);
	}
}

}// namespace Pomdog
