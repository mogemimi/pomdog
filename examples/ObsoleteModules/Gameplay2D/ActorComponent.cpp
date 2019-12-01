// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay2D/ActorComponent.hpp"
#include "Pomdog.Experimental/Gameplay/detail/ComponentTypeIndex.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

void ActorComponent::RunAction(std::unique_ptr<Action>&& action)
{
    POMDOG_ASSERT(action);
    actions.push_back(std::move(action));
}

void ActorComponent::Act(Entity& entity, const Skeletal2D::AnimationTimeInterval& frameDuration)
{
    POMDOG_ASSERT(entity);
    if (actions.empty()) {
        return;
    }

    for (auto& action : actions) {
        POMDOG_ASSERT(action);
        action->Act(entity, frameDuration);
        if (!entity) {
            break;
        }
    }

    actions.erase(std::remove_if(std::begin(actions), std::end(actions),
        [](const std::unique_ptr<Action>& action) {
            POMDOG_ASSERT(action);
            return action->IsCompleted();
        }),
        std::end(actions));
}

void ActorComponent::StopActions()
{
    actions.clear();
}

std::uint8_t ComponentTypeDeclaration<ActorComponent>::GetTypeIndex()
{
    return Detail::Gameplay::ComponentTypeIndex::Index<ActorComponent>();
}

std::shared_ptr<Component> ComponentCreator<ActorComponent>::CreateComponent()
{
    auto component = std::make_shared<ActorComponent>();
    return component;
}

std::uint8_t ComponentCreator<ActorComponent>::GetComponentType()
{
    return ComponentTypeDeclaration<ActorComponent>::GetTypeIndex();
}

} // namespace Pomdog
