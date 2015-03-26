// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP
#define POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <string>
#include <memory>

namespace Pomdog {

class Skeleton;
class SkeletonTransform;
class AnimationGraph;

class Animator: public Component<Animator> {
public:
    Animator(std::shared_ptr<Skeleton> const& skeleton,
        std::shared_ptr<SkeletonTransform> const& skeletonTransform,
        std::shared_ptr<AnimationGraph> const& animationGraph);

    ~Animator();

    void Update(Duration const& frameDuration);

    void CrossFade(std::string const& state, Duration const& transitionDuration);

    void Play(std::string const& state);

    float PlaybackRate() const;
    void PlaybackRate(float playbackRate);

    void SetFloat(std::string const& name, float value);
    void SetBool(std::string const& name, bool value);

    std::string GetCurrentStateName() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATOR_0CFC0E37_CCD5_481A_9275_E8579887ACB0_HPP)
