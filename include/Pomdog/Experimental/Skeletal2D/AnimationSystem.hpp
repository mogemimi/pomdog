// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/GameClock.hpp"
#include <memory>

namespace Pomdog::Skeletal2D {

class AnimationState;
class Skeleton;
class SkeletonPose;
class Skin;

class AnimationSystem final {
public:
    AnimationSystem();
    ~AnimationSystem();

    void Add(
        const std::shared_ptr<AnimationState>& state,
        const std::shared_ptr<Skeleton const>& skeleton,
        const std::shared_ptr<SkeletonPose>& skeletonPose,
        const std::shared_ptr<Skin>& skin);

    void Remove(const std::shared_ptr<AnimationState>& state);

    void Update(const GameClock& clock);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog::Skeletal2D
