//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay/GameWorld.hpp>
#include <Pomdog/Gameplay/GameObject.hpp>

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
	return context->Valid(objectID);
}
//-----------------------------------------------------------------------
void GameWorld::RemoveUnusedObjects()
{
	POMDOG_ASSERT_MESSAGE(false, "TODO: Not implemented");

	objects.erase(std::remove_if(std::begin(objects), std::end(objects),
		[this](GameObjectID const& id){ return context->Valid(id); }), std::end(objects));
}
//-----------------------------------------------------------------------
}// namespace Pomdog
