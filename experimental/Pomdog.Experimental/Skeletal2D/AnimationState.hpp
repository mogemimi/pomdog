// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTimeInterval.hpp"
#include <memory>

namespace Pomdog {

class AnimationClip;

class AnimationState {
public:
    explicit AnimationState(std::shared_ptr<AnimationClip> const& animationClip);
    AnimationState(std::shared_ptr<AnimationClip> const& animationClip, float playbackRate, bool loop);

    void Update(AnimationTimeInterval const& frameDuration);

    std::shared_ptr<AnimationClip> const& Clip() const;

    AnimationTimeInterval Time() const;
    void Time(AnimationTimeInterval const& time);

    AnimationTimeInterval Length() const;

    float PlaybackRate() const;
    void PlaybackRate(float playbackRate);

    bool Enabled() const;
    void Enabled(bool enabled);

    bool Loop() const;
    void Loop(bool loop);

private:
    std::shared_ptr<AnimationClip> clip;
    AnimationTimeInterval time;
    float playbackRate;
    //float weight;
    bool enabled;
    bool loop;
};

} // namespace Pomdog
