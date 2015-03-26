// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP
#define POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP

#include "Pomdog.Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class AnimationTimer {
public:
    AnimationTimer();

    void Update(AnimationTimeInterval const& time);
    void Pause();
    void Resume();
    void Reset();

    AnimationTimeInterval Time() const;
    bool IsPlaying() const;

private:
    AnimationTimeInterval time;
    bool isPlaying;
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONTIMER_A9EAE898_56A1_4928_B031_22D9304214E6_HPP)
