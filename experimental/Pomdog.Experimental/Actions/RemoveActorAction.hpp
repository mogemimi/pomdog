// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Actions/Action.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

class RemoveActorAction: public Action {
private:
    bool isCompleted = false;

public:
    void Act(GameObject & gameObject, AnimationTimeInterval const&) override
    {
        if (isCompleted) {
            return;
        }

        POMDOG_ASSERT(gameObject);

        if (gameObject) {
            gameObject.Destroy();
        }

        isCompleted = true;
    }

    bool IsCompleted() const override
    {
        return isCompleted;
    }
};

} // namespace Pomdog
