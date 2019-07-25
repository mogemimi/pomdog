// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include <memory>

namespace Pomdog {

class Skeleton;
class SkeletonPose;
class AnimationState;

class AnimationSystem {
public:
    AnimationSystem();
    ~AnimationSystem();

    void Add(std::shared_ptr<AnimationState> const& state,
        std::shared_ptr<Skeleton const> const& skeleton,
        std::shared_ptr<SkeletonPose> const& skeletonPose);

    void Remove(std::shared_ptr<AnimationState> const& state);

    void Update(GameClock const& clock);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog
