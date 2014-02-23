//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/GamePlay/GameObject.hpp>

namespace Pomdog {

GameObject::GameObject(std::uint64_t instanceIDIn)
	: instanceID(instanceIDIn)
{}
//-----------------------------------------------------------------------
GameObject::GameObject(std::uint64_t instanceIDIn, std::size_t minimumCapacity)
	: instanceID(instanceIDIn)
{
	components.reserve(minimumCapacity);
}
//-----------------------------------------------------------------------
std::uint64_t GameObject::InstanceID() const
{
	return instanceID;
}

}// namespace Pomdog
