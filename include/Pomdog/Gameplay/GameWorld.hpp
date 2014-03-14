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
namespace ComponentQuery {

template <typename T>
struct Not {};

}// namespace ComponentQuery

namespace Details {
namespace Gameplay {

template <typename...T>
struct HasComponents;

template <typename T>
struct HasComponents<T>
{
	bool operator()(GameObject const& gameObject)
	{
		return gameObject.HasComponent<T>();
	}
};

template <typename T>
struct HasComponents<ComponentQuery::Not<T>>
{
	bool operator()(GameObject const& gameObject)
	{
		return !gameObject.HasComponent<T>();
	}
};

template <typename T, typename...Arguments>
struct HasComponents<T, Arguments...>
{
	bool operator()(GameObject const& gameObject)
	{
		return gameObject.HasComponent<T>() && HasComponents<Arguments...>()(gameObject);
	}
};

}// namespace Gameplay
}// namespace Details

class GameObject;

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay
/// @{

class POMDOG_EXPORT GameWorld {
public:
	GameWorld();
	
	GameWorld(GameWorld const&) = delete;
	GameWorld(GameWorld &&) = default;
	
	GameWorld & operator=(GameWorld const&) = delete;
	GameWorld & operator=(GameWorld &&) = default;

	std::shared_ptr<GameObject> CreateObject();
	
	template <typename T, typename...Components>
	std::vector<std::shared_ptr<GameObject>> QueryComponents();
	
	template <typename T>
	T const* Component(GameObjectID objectID) const;
	
	template <typename T>
	T* Component(GameObjectID objectID);
	
private:
	std::vector<std::shared_ptr<GameObject>> gameObjects;
	std::shared_ptr<GameObjectContext> objectContext;
};

/// @}
/// @}


template <typename T, typename...Components>
std::vector<std::shared_ptr<GameObject>> GameWorld::QueryComponents()
{
	static_assert(std::is_object<T>::value, "");
	
	std::vector<std::shared_ptr<GameObject>> result;

	for (auto & gameObject: gameObjects)
	{
		if (Details::Gameplay::HasComponents<T, Components...>()(*gameObject)) {
			result.push_back(gameObject);
		}
	}
	return std::move(result);
}

template <typename T>
T const* GameWorld::Component(GameObjectID objectID) const
{
	static_assert(std::is_object<T>::value, "");
	POMDOG_ASSERT(objectContext);
	return objectContext->Component<T>(objectID);
}

template <typename T>
T* GameWorld::Component(GameObjectID objectID)
{
	static_assert(std::is_object<T>::value, "");
	POMDOG_ASSERT(objectContext);
	return objectContext->Component<T>(objectID);
}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEWORLD_FE6556F6_01FB_4EF9_A702_E5EB80BE3512_HPP)
