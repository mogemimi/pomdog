// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/EntityManager.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
EntityManager::EntityManager()
    : context(std::make_shared<EntityContext>())
{}
//-----------------------------------------------------------------------
Entity EntityManager::CreateObject()
{
    POMDOG_ASSERT(context);
    Entity entity {context};
    objects.push_back(entity.GetEntityID());
    return std::move(entity);
}
//-----------------------------------------------------------------------
bool EntityManager::Valid(EntityID const& objectID) const
{
    POMDOG_ASSERT(context);
    return context->Valid(objectID);
}
//-----------------------------------------------------------------------
void EntityManager::Refresh()
{
    POMDOG_ASSERT(context);
    context->Refresh();

    objects.erase(std::remove_if(std::begin(objects), std::end(objects),
        [this](EntityID const& id){ return !context->Valid(id); }),
        std::end(objects));
}
//-----------------------------------------------------------------------
std::size_t EntityManager::GetCount() const noexcept
{
    POMDOG_ASSERT(context);
    return context->GetCount();
}
//-----------------------------------------------------------------------
std::size_t EntityManager::GetCapacity() const noexcept
{
    POMDOG_ASSERT(context);
    return context->GetCapacity();
}
//-----------------------------------------------------------------------
void EntityManager::Clear()
{
    POMDOG_ASSERT(context);
    context->Clear();
}
//-----------------------------------------------------------------------
} // namespace Pomdog
