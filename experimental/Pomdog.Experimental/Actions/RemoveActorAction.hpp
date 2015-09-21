// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_REMOVEACTORACTION_4E4B9484_HPP
#define POMDOG_REMOVEACTORACTION_4E4B9484_HPP

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

#endif // POMDOG_REMOVEACTORACTION_4E4B9484_HPP
