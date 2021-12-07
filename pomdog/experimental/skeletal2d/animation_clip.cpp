// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animation_clip.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/skeleton.hpp"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
