// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/game_clock.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

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

} // namespace pomdog::skeletal2d
