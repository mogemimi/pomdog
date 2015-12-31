// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog.Experimental/Gameplay/GameObject.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> const& contextIn)
    : context(contextIn)
    , id(context->Create())
{}
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> && contextIn)
    : context(std::move(contextIn))
    , id(context->Create())
{}
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> const& contextIn, GameObjectID const& idIn)
    : context(contextIn)
    , id(idIn)
{}
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> && contextIn, GameObjectID const& idIn)
    : context(std::move(contextIn))
    , id(idIn)
{}
//-----------------------------------------------------------------------
GameObject::operator bool() const
{
    return context && context->Valid(id);
}
//-----------------------------------------------------------------------
bool GameObject::operator==(GameObject const& gameObject) const
{
    POMDOG_ASSERT((!context || !gameObject.context) || context == gameObject.context);
    return (context && gameObject.context) && (id == gameObject.id);
}
//-----------------------------------------------------------------------
bool GameObject::operator!=(GameObject const& gameObject) const
{
    POMDOG_ASSERT((!context || !gameObject.context) || context == gameObject.context);
    return (!context || !gameObject.context) || (id != gameObject.id);
}
//-----------------------------------------------------------------------
void GameObject::Destroy()
{
    if (context && context->Valid(id)) {
        context->Destroy(id);
    }
}
//-----------------------------------------------------------------------
void GameObject::DestroyImmediate()
{
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));

    if (context->Valid(id))
    {
        context->DestroyImmediate(id);
    }
}
//-----------------------------------------------------------------------
GameObjectID GameObject::ID() const
{
    POMDOG_ASSERT(context && context->Valid(id));
    return id;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
