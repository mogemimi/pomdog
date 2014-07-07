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
std::shared_ptr<GameObject> GameWorld::CreateObject()
{
	POMDOG_ASSERT(context);
	auto gameObject = std::make_shared<GameObject>(context);
	gameObjects.push_back(gameObject);
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
	gameObjects.erase(std::remove_if(std::begin(gameObjects), std::end(gameObjects),
		[](std::weak_ptr<GameObject> const& gameObject){ return gameObject.expired(); }), std::end(gameObjects));
}
//-----------------------------------------------------------------------
}// namespace Pomdog
