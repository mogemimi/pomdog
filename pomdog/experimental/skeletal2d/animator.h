// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/chrono/duration.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationGraph;
class Skeleton;
class SkeletonPose;

class Animator final {
public:
    Animator(
        const std::shared_ptr<Skeleton>& skeleton,
        const std::shared_ptr<skeletal2d::SkeletonPose>& skeletonPose,
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

} // namespace pomdog::skeletal2d
