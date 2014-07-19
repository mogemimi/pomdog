//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEWORLD_FE6556F6_01FB_4EF9_A702_E5EB80BE3512_HPP
#define POMDOG_GAMEWORLD_FE6556F6_01FB_4EF9_A702_E5EB80BE3512_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include <vector>
#include "../Config/Export.hpp"
#include "GameObject.hpp"

namespace Pomdog {

class GameObject;

class POMDOG_EXPORT GameWorld {
public:
	GameWorld();
	
	GameWorld(GameWorld const&) = delete;
	GameWorld(GameWorld &&) = default;
	
	GameWorld & operator=(GameWorld const&) = delete;
	GameWorld & operator=(GameWorld &&) = default;

	GameObject CreateObject();
	
	//template <typename T, typename...Components>
	//std::vector<T> QueryComponent();
	
	template <typename T, typename...Components>
	std::vector<GameObject> QueryComponents();
	
	template <typename T>
	T const* Component(GameObjectID const& objectID) const;
	
	template <typename T>
	T* Component(GameObjectID const& objectID);
	
	template <typename T>
	bool HasComponent(GameObjectID const& objectID) const;
	
	bool Valid(GameObjectID const& objectID) const;
	
	void RemoveUnusedObjects();
	
private:
	std::shared_ptr<GameObjectContext> context;
	std::vector<GameObjectID> objects;
};


namespace Details {
namespace Gameplay {

template <typename...T>
struct HasComponents;

template <typename T>
struct HasComponents<T>
{
	bool operator()(GameObjectContext const& context, GameObjectID const& id)
	{
		return context.HasComponent<T>(id);
	}
};

template <typename T, typename...Arguments>
struct HasComponents<T, Arguments...>
{
	bool operator()(GameObjectContext const& context, GameObjectID const& id)
	{
		return context.HasComponent<T>(id) && HasComponents<Arguments...>()(context, id);
	}
};

}// namespace Gameplay
}// namespace Details

template <typename T, typename...Components>
std::vector<GameObject> GameWorld::QueryComponents()
{
	static_assert(std::is_object<T>::value, "");

	std::vector<GameObject> result;

	for (auto & id: objects)
	{
		if (context->Valid(id)) {
			if (Details::Gameplay::HasComponents<T, Components...>()(*context, id)) {
				result.emplace_back(context, id);
			}
		}
	}
	return std::move(result);
}

template <typename T>
T const* GameWorld::Component(GameObjectID const& id) const
{
	static_assert(std::is_object<T>::value, "");
	POMDOG_ASSERT(context);
	return context->Component<T>(id);
}

template <typename T>
T* GameWorld::Component(GameObjectID const& id)
{
	static_assert(std::is_object<T>::value, "");
	POMDOG_ASSERT(context);
	return context->Component<T>(id);
}

template <typename T>
bool GameWorld::HasComponent(GameObjectID const& id) const
{
	static_assert(std::is_object<T>::value, "");
	POMDOG_ASSERT(context);
	return context->HasComponent<T>(id);
}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWORLD_FE6556F6_01FB_4EF9_A702_E5EB80BE3512_HPP)
