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
	: incrementalObjectID(0)
{}
//-----------------------------------------------------------------------
std::shared_ptr<GameObject> GameWorld::CreateObject()
{
	++incrementalObjectID.value;
	auto gameObject = std::make_shared<GameObject>(incrementalObjectID);
	gameObjects.push_back(gameObject);
	return std::move(gameObject);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
