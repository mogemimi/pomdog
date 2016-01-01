// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Actions/Action.hpp"

namespace Pomdog {
namespace Detail {
namespace Actions {

class InterpolationLinear {
public:
    float Apply(float normalizedTime)
    {
        return normalizedTime;
    }
};

template <typename ActionClass>
class TemporalAction: public Action {
private:
    AnimationTimeInterval const duration;
    AnimationTimeInterval time;
    bool isCompleted;
    bool began;

    ActionClass internalAction;
    InterpolationLinear interpolation;

public:
    template <typename...Args>
    TemporalAction(AnimationTimeInterval const& durationIn, Args &&...args)
        : duration(durationIn)
        , time(AnimationTimeInterval::zero())
        , isCompleted(false)
        , began(false)
        , internalAction(std::forward<Args>(args)...)
    {}

    void Act(Entity & entity, AnimationTimeInterval const& frameDuration) override
    {
        if (isCompleted) {
            return;
        }

        if (!began) {
            internalAction.Begin(entity);
            began = true;
        }

        time += frameDuration;
        float normalizedTime = time.count() / duration.count();

        if (time >= duration) {
            isCompleted = true;
            normalizedTime = 1.f;
        } else {
            normalizedTime = interpolation.Apply(normalizedTime);
        }

        internalAction.Update(entity, normalizedTime);
    }

    bool IsCompleted() const override
    {
        return isCompleted;
    }

    AnimationTimeInterval Time() const
    {
        return time;
    }

    AnimationTimeInterval Duration() const
    {
        return duration;
    }
};

} // namespace Actions
} // namespace Detail
} // namespace Pomdog
