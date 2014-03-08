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
	: objectContext(std::make_shared<GameObjectContext>())
{}
//-----------------------------------------------------------------------
std::shared_ptr<GameObject> GameWorld::CreateObject()
{
	POMDOG_ASSERT(objectContext);
	auto gameObject = std::make_shared<GameObject>(objectContext);
	gameObjects.push_back(gameObject);
	return std::move(gameObject);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
