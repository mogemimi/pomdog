// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/Entity.hpp"

namespace Pomdog {

Entity::Entity(EntityContext* contextIn, const EntityID& idIn) noexcept
    : context(contextIn)
    , id(idIn)
{}

Entity::operator bool() const noexcept
{
    return (context != nullptr) && context->Valid(id);
}

bool Entity::operator==(const Entity& other) const noexcept
{
    return (context == other.context) && (id == other.id);
}

bool Entity::operator!=(const Entity& other) const noexcept
{
    return (context != other.context) || (id != other.id);
}

void Entity::Destroy()
{
    if ((context != nullptr) && context->Valid(id)) {
        context->Destroy(id);
    }
    context = nullptr;
    id = EntityID{0, 0};
}

void Entity::DestroyImmediate()
{
    if ((context != nullptr) && context->Valid(id)) {
        context->DestroyImmediate(id);
    }
    context = nullptr;
    id = EntityID{0, 0};
}

EntityID Entity::GetID() const noexcept
{
    POMDOG_ASSERT((context != nullptr) && context->Valid(id));
    return id;
}

} // namespace Pomdog
