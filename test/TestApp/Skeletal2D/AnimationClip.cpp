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
#include "AnimationTrack.hpp"

namespace Pomdog {

AnimationClip::AnimationClip(std::vector<std::unique_ptr<AnimationTrack>> && tracksIn)
	: tracks(std::move(tracksIn))
{}
//-----------------------------------------------------------------------
void AnimationClip::Apply(DurationSeconds const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	for (auto & track: tracks)
	{
		POMDOG_ASSERT(track);
		track->Apply(time, skeleton, skeletonPose);
	}
}
//-----------------------------------------------------------------------
DurationSeconds AnimationClip::Length() const
{
	DurationSeconds maxLength(0);
	
	for (auto & track: tracks)
	{
		POMDOG_ASSERT(track);
		auto length = track->Length();
		
		if (length > maxLength)
		{
			maxLength = length;
		}
	}
	return maxLength;
}

}// namespace Pomdog
