// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog.Experimental/Gameplay/GameWorld.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
GameWorld::GameWorld()
    : context(std::make_shared<GameObjectContext>())
{}
//-----------------------------------------------------------------------
GameObject GameWorld::CreateObject()
{
    POMDOG_ASSERT(context);
    GameObject gameObject {context};
    objects.push_back(gameObject.ID());
    return std::move(gameObject);
}
//-----------------------------------------------------------------------
bool GameWorld::Valid(GameObjectID const& objectID) const
{
    POMDOG_ASSERT(context);
    return context->Valid(objectID);
}
//-----------------------------------------------------------------------
void GameWorld::Refresh()
{
    POMDOG_ASSERT(context);
    context->Refresh();

    objects.erase(std::remove_if(std::begin(objects), std::end(objects),
        [this](GameObjectID const& id){ return !context->Valid(id); }), std::end(objects));
}
//-----------------------------------------------------------------------
std::size_t GameWorld::Count() const
{
    POMDOG_ASSERT(context);
    return context->Count();
}
//-----------------------------------------------------------------------
std::size_t GameWorld::Capacity() const
{
    POMDOG_ASSERT(context);
    return context->Capacity();
}
//-----------------------------------------------------------------------
void GameWorld::Clear()
{
    POMDOG_ASSERT(context);
    context->Clear();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
