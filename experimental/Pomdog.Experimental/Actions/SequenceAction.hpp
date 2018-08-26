// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Actions/Action.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cstdint>

namespace Pomdog {

class SequenceAction final : public Action {
private:
    std::array<std::unique_ptr<Action>, 4> actions;
    int const actionCount;
    int index;
    bool isCompleted;

public:
    SequenceAction(std::unique_ptr<Action> && action1)
        : actions({{std::move(action1)}})
        , actionCount(1)
        , index(0)
        , isCompleted(false)
    {}

    SequenceAction(
        std::unique_ptr<Action> && action1,
        std::unique_ptr<Action> && action2)
        : actions({{std::move(action1), std::move(action2)}})
        , actionCount(2)
        , index(0)
        , isCompleted(false)
    {}

    SequenceAction(
        std::unique_ptr<Action> && action1,
        std::unique_ptr<Action> && action2,
        std::unique_ptr<Action> && action3)
        : actions({{std::move(action1), std::move(action2), std::move(action3)}})
        , actionCount(3)
        , index(0)
        , isCompleted(false)
    {}

    SequenceAction(
        std::unique_ptr<Action> && action1,
        std::unique_ptr<Action> && action2,
        std::unique_ptr<Action> && action3,
        std::unique_ptr<Action> && action4)
        : actions({{std::move(action1), std::move(action2), std::move(action3), std::move(action4)}})
        , actionCount(4)
        , index(0)
        , isCompleted(false)
    {}

    void Act(Entity & entity, AnimationTimeInterval const& frameDuration) override
    {
        if (isCompleted) {
            return;
        }

        POMDOG_ASSERT(index >= 0);
        POMDOG_ASSERT(index < static_cast<int>(actions.size()));
        POMDOG_ASSERT(index < actionCount);

        auto & action = actions[index];

        POMDOG_ASSERT(action);
        action->Act(entity, frameDuration);

        if (action->IsCompleted()) {
            POMDOG_ASSERT(index < actionCount);
            ++index;
        }

        if (index >= actionCount) {
            isCompleted = true;
        }
    }

    bool IsCompleted() const override
    {
        return isCompleted;
    }
};

} // namespace Pomdog
