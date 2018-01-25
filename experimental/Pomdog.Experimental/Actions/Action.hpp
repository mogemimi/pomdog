// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog {

class Entity;

class Action {
public:
    virtual ~Action() = default;

    virtual void Act(Entity & entity, AnimationTimeInterval const& duration) = 0;

    virtual bool IsCompleted() const = 0;
};

} // namespace Pomdog
