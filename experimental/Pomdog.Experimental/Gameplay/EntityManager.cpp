// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/EntityManager.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"

namespace Pomdog {

Entity EntityManager::CreateEntity(
    std::vector<std::shared_ptr<ComponentCreatorBase>> && componentCreators)
{
    Entity entity(&context, context.Create(std::move(componentCreators)));
    entities.push_back(entity.GetID());
    return std::move(entity);
}

bool EntityManager::Valid(const EntityID& objectID) const noexcept
{
    return context.Valid(objectID);
}

void EntityManager::Refresh()
{
    const auto entityCount = context.GetCount();
    context.Refresh();

    if (entityCount != context.GetCount()) {
        entities.erase(std::remove_if(std::begin(entities), std::end(entities),
            [this](EntityID const& id){ return !context.Valid(id); }),
            std::end(entities));
    }
}

std::size_t EntityManager::GetCount() const noexcept
{
    return context.GetCount();
}

std::size_t EntityManager::GetCapacity() const noexcept
{
    return context.GetCapacity();
}

void EntityManager::Clear()
{
    context.Clear();
}

} // namespace Pomdog
