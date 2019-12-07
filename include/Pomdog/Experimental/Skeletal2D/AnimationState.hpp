// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include <memory>

namespace Pomdog::Skeletal2D {

class AnimationClip;

class AnimationState final {
public:
    explicit AnimationState(const std::shared_ptr<AnimationClip>& animationClip);
    AnimationState(const std::shared_ptr<AnimationClip>& animationClip, float playbackRate, bool loop);

    void Update(const AnimationTimeInterval& frameDuration);

    const std::shared_ptr<AnimationClip>& GetClip() const;

    AnimationTimeInterval GetTime() const noexcept;
    void SetTime(const AnimationTimeInterval& time) noexcept;

    AnimationTimeInterval GetLength() const;

    float GetPlaybackRate() const noexcept;
    void SetPlaybackRate(float playbackRate) noexcept;

    bool IsEnabled() const noexcept;
    void SetEnabled(bool enabled) noexcept;

    bool IsLoop() const noexcept;
    void SetLoop(bool loop) noexcept;

private:
    std::shared_ptr<AnimationClip> clip;
    AnimationTimeInterval time;
    float playbackRate;
    //float weight;
    bool enabled;
    bool loop;
};

} // namespace Pomdog::Skeletal2D
