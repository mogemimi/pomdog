//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "AnimationClip.hpp"
#include "AnimationTrack.hpp"
#include "Skeleton.hpp"
#include <utility>

namespace Pomdog {
namespace {

static AnimationTimeInterval ComputeLength(std::vector<std::unique_ptr<AnimationTrack>> const& tracks)
{
	AnimationTimeInterval maxLength = AnimationTimeInterval::zero();

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

}// unnamed namespace
//-----------------------------------------------------------------------
AnimationClip::AnimationClip(std::vector<std::unique_ptr<AnimationTrack>> && tracksIn)
	: tracks(std::move(tracksIn))
{
	length = ComputeLength(tracks);
}
//-----------------------------------------------------------------------
void AnimationClip::Apply(AnimationTimeInterval const& time, Skeleton const& skeleton, SkeletonPose & skeletonPose)
{
	for (auto & track: tracks)
	{
		POMDOG_ASSERT(track);
		track->Apply(time, skeleton, skeletonPose);
	}
}
//-----------------------------------------------------------------------
AnimationTimeInterval AnimationClip::Length() const
{
	return length;
}

}// namespace Pomdog
