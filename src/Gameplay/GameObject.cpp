//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Gameplay/GameObject.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> const& contextIn)
	: context(contextIn)
	, id(contextIn->Create())
{
}
//-----------------------------------------------------------------------
GameObject::GameObject(std::shared_ptr<GameObjectContext> && contextIn)
	: context(std::move(contextIn))
	, id(context->Create())
{
}
//-----------------------------------------------------------------------
GameObject::~GameObject()
{
	context->Destroy(id);
}
//-----------------------------------------------------------------------
GameObjectID GameObject::ID() const
{
	return id;
}
//-----------------------------------------------------------------------
}// namespace Pomdog
