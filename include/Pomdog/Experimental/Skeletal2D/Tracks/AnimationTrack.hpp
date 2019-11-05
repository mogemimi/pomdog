// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog::Skeletal2D {

class Skeleton;
class SkeletonPose;
class Skin;

class AnimationTrack {
public:
    virtual ~AnimationTrack() = default;

    virtual void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) = 0;

    virtual AnimationTimeInterval GetLength() const = 0;
};

} // namespace Pomdog::Skeletal2D
