// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "AnimationTimer.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

AnimationTimer::AnimationTimer()
    : time(AnimationTimeInterval::zero())
    , isPlaying(true)
{
}

void AnimationTimer::Update(AnimationTimeInterval const& timeIn)
{
    if (isPlaying)
    {
        time += timeIn;
    }
}

void AnimationTimer::Reset()
{
    time = AnimationTimeInterval::zero();
}

void AnimationTimer::Pause()
{
    isPlaying = false;
}

void AnimationTimer::Resume()
{
    isPlaying = true;
}

AnimationTimeInterval AnimationTimer::Time() const
{
    return time;
}

bool AnimationTimer::IsPlaying() const
{
    return isPlaying;
}

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
