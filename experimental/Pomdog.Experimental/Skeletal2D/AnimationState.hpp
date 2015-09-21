// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONSTATE_35B967B3_HPP
#define POMDOG_ANIMATIONSTATE_35B967B3_HPP

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

#endif // POMDOG_ANIMATIONSTATE_35B967B3_HPP
