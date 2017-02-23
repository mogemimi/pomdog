// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

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

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
