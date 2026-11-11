// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/skeletal2d/skeleton.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

AnimationTimeInterval
computeLength(const std::vector<std::unique_ptr<AnimationTrack>>& tracks)
{
    AnimationTimeInterval maxLength = AnimationTimeInterval::zero();

    for (auto& track : tracks) {
        POMDOG_ASSERT(track);
        auto length_ = track->getLength();

        if (length_ > maxLength) {
            maxLength = length_;
        }
    }
    return maxLength;
}

} // namespace

AnimationClip::AnimationClip(std::vector<std::unique_ptr<AnimationTrack>>&& tracksIn)
    : tracks_(std::move(tracksIn))
{
    length_ = computeLength(tracks_);
}

void AnimationClip::apply(
    const AnimationTimeInterval& time,
    const Skeleton& skeleton,
    SkeletonPose& skeletonPose,
    Skin* skin)
{
    for (auto& track : tracks_) {
        POMDOG_ASSERT(track);
        track->apply(time, skeleton, skeletonPose, skin);
    }
}

AnimationTimeInterval AnimationClip::getLength() const
{
    return length_;
}

} // namespace pomdog::skeletal2d
