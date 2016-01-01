// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/Entity.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
Entity::Entity(std::shared_ptr<EntityContext> const& contextIn)
    : context(contextIn)
    , id(context->Create())
{}
//-----------------------------------------------------------------------
Entity::Entity(std::shared_ptr<EntityContext> && contextIn)
    : context(std::move(contextIn))
    , id(context->Create())
{}
//-----------------------------------------------------------------------
Entity::Entity(std::shared_ptr<EntityContext> const& contextIn, EntityID const& idIn)
    : context(contextIn)
    , id(idIn)
{}
//-----------------------------------------------------------------------
Entity::Entity(std::shared_ptr<EntityContext> && contextIn, EntityID const& idIn)
    : context(std::move(contextIn))
    , id(idIn)
{}
//-----------------------------------------------------------------------
Entity::operator bool() const
{
    return context && context->Valid(id);
}
//-----------------------------------------------------------------------
bool Entity::operator==(Entity const& entity) const
{
    POMDOG_ASSERT((!context || !entity.context) || context == entity.context);
    return (context && entity.context) && (id == entity.id);
}
//-----------------------------------------------------------------------
bool Entity::operator!=(Entity const& entity) const
{
    POMDOG_ASSERT((!context || !entity.context) || context == entity.context);
    return (!context || !entity.context) || (id != entity.id);
}
//-----------------------------------------------------------------------
void Entity::Destroy()
{
    if (context && context->Valid(id)) {
        context->Destroy(id);
    }
}
//-----------------------------------------------------------------------
void Entity::DestroyImmediate()
{
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));

    if (context->Valid(id)) {
        context->DestroyImmediate(id);
    }
}
//-----------------------------------------------------------------------
EntityID Entity::ID() const
{
    POMDOG_ASSERT(context && context->Valid(id));
    return id;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
