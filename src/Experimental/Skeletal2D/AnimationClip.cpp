// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/AnimationClip.hpp"
#include "Pomdog/Experimental/Skeletal2D/Skeleton.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"
#include <utility>

namespace Pomdog::Skeletal2D {
namespace {

AnimationTimeInterval
ComputeLength(const std::vector<std::unique_ptr<AnimationTrack>>& tracks)
{
    AnimationTimeInterval maxLength = AnimationTimeInterval::zero();

    for (auto& track : tracks) {
        POMDOG_ASSERT(track);
        auto length = track->GetLength();

        if (length > maxLength) {
            maxLength = length;
        }
    }
    return maxLength;
}

} // namespace

AnimationClip::AnimationClip(std::vector<std::unique_ptr<AnimationTrack>>&& tracksIn)
    : tracks(std::move(tracksIn))
{
    length = ComputeLength(tracks);
}

void AnimationClip::Apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin)
{
    for (auto& track : tracks) {
        POMDOG_ASSERT(track);
        track->Apply(time, skeleton, skeletonPose, skin);
    }
}

AnimationTimeInterval AnimationClip::GetLength() const
{
    return length;
}

} // namespace Pomdog::Skeletal2D
