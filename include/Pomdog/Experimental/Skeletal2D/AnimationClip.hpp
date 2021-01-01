// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class Skeleton;
class SkeletonPose;
class Skin;

class AnimationClip final {
public:
    AnimationClip() = default;

    explicit AnimationClip(std::vector<std::unique_ptr<AnimationTrack>>&& tracks);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin);

    AnimationTimeInterval GetLength() const;

private:
    std::vector<std::unique_ptr<AnimationTrack>> tracks;
    AnimationTimeInterval length;
};

} // namespace Pomdog::Skeletal2D
