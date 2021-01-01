// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class AnimationGraph;
class Skeleton;
class SkeletonPose;

class Animator final {
public:
    Animator(
        const std::shared_ptr<Skeleton>& skeleton,
        const std::shared_ptr<Skeletal2D::SkeletonPose>& skeletonPose,
        const std::shared_ptr<AnimationGraph>& animationGraph);

    ~Animator();

    void Update(const Duration& frameDuration);

    void CrossFade(const std::string& state, const Duration& transitionDuration);

    void Play(const std::string& state);

    float GetPlaybackRate() const noexcept;

    void SetPlaybackRate(float playbackRate) noexcept;

    void SetFloat(const std::string& name, float value);

    void SetBool(const std::string& name, bool value);

    std::string GetCurrentStateName() const noexcept;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog::Skeletal2D
