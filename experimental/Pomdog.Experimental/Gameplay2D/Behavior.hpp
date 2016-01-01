// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <memory>

namespace Pomdog {

class Entity;

class Behavior: public Component<Behavior> {
public:
    virtual ~Behavior() = default;

    virtual void Start(Entity & entity) = 0;

    virtual void Update(Entity & entity, Duration const& frameDuration) = 0;
};

} // namespace Pomdog
