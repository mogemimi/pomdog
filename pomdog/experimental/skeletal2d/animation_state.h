// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/animation_time_interval.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class AnimationClip;

class AnimationState final {
public:
    explicit AnimationState(const std::shared_ptr<AnimationClip>& animationClip);
    AnimationState(const std::shared_ptr<AnimationClip>& animationClip, f32 playbackRate, bool loop);

    void update(const AnimationTimeInterval& frameDuration);

    const std::shared_ptr<AnimationClip>& getClip() const;

    AnimationTimeInterval getTime() const noexcept;
    void setTime(const AnimationTimeInterval& time) noexcept;

    AnimationTimeInterval getLength() const;

    f32 getPlaybackRate() const noexcept;
    void setPlaybackRate(f32 playbackRate) noexcept;

    bool isEnabled() const noexcept;
    void setEnabled(bool enabled) noexcept;

    bool isLoop() const noexcept;
    void setLoop(bool loop) noexcept;

private:
    std::shared_ptr<AnimationClip> clip_;
    AnimationTimeInterval time_;
    f32 playbackRate_;
    //float weight;
    bool enabled_;
    bool loop_;
};

} // namespace pomdog::skeletal2d
