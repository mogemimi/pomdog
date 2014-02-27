//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/GamePlay/GameObject.hpp>

namespace Pomdog {

GameObject::GameObject(GameObjectID const& instanceIDIn)
	: instanceID(instanceIDIn)
{}
//-----------------------------------------------------------------------
GameObject::GameObject(GameObjectID && instanceIDIn)
	: instanceID(std::move(instanceIDIn))
{}
//-----------------------------------------------------------------------
GameObject::GameObject(GameObjectID const& instanceIDIn, std::size_t minimumCapacity)
	: instanceID(instanceIDIn)
{
	components.reserve(minimumCapacity);
}
//-----------------------------------------------------------------------
GameObject::GameObject(GameObjectID && instanceIDIn, std::size_t minimumCapacity)
	: instanceID(std::move(instanceIDIn))
{
	components.reserve(minimumCapacity);
}
//-----------------------------------------------------------------------
GameObjectID const& GameObject::ID() const
{
	return instanceID;
}

}// namespace Pomdog
