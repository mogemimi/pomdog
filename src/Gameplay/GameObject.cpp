//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay/GameObject.hpp>

namespace Pomdog {

GameObject::GameObject(GameObjectID const& instanceIDIn)
	: instanceID(instanceIDIn)
{
	components.reserve(4);
}
//-----------------------------------------------------------------------
GameObject::GameObject(GameObjectID && instanceIDIn)
	: instanceID(std::move(instanceIDIn))
{
	components.reserve(4);
}
//-----------------------------------------------------------------------
GameObjectID const& GameObject::ID() const
{
	return instanceID;
}

}// namespace Pomdog
