// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Skeletal2D/AnimationTimeInterval.hpp"

namespace Pomdog {

class GameObject;

class Action {
public:
    virtual ~Action() = default;

    virtual void Act(GameObject & gameObject, AnimationTimeInterval const& duration) = 0;

    virtual bool IsCompleted() const = 0;
};

} // namespace Pomdog
