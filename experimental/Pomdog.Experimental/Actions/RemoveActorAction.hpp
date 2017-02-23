// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Actions/Action.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

class RemoveActorAction final : public Action {
private:
    bool isCompleted = false;

public:
    void Act(Entity & entity, AnimationTimeInterval const&) override
    {
        if (isCompleted) {
            return;
        }

        POMDOG_ASSERT(entity);
        if (entity) {
            entity.Destroy();
        }
        isCompleted = true;
    }

    bool IsCompleted() const override
    {
        return isCompleted;
    }
};

} // namespace Pomdog
