// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ACTION_F54B5D22_HPP
#define POMDOG_ACTION_F54B5D22_HPP

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

#endif // POMDOG_ACTION_F54B5D22_HPP
