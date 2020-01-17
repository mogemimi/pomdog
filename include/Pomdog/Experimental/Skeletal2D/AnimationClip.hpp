// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"
#include <memory>
#include <vector>

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
