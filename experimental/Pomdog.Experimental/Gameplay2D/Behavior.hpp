// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <memory>

namespace Pomdog {

class GameObject;

class Behavior: public Component<Behavior> {
public:
    virtual ~Behavior() = default;

    virtual void Start(GameObject & gameObject) = 0;

    virtual void Update(GameObject & gameObject, Duration const& frameDuration) = 0;
};

} // namespace Pomdog
