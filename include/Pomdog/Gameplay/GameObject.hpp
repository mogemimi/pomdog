//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP
#define POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>
#include "../Config/Export.hpp"
#include "../Utility/Assert.hpp"
#include "detail/GameComponent.hpp"
#include "GameObjectID.hpp"
#include "GameObjectContext.hpp"

namespace Pomdog {

///@~Japanese
/// @brief コンポーネントを保持するゲームオブジェクトです。複数のコンポーネントを格納するコンテナの役割を担います。
class POMDOG_EXPORT GameObject {
public:
	GameObject() = default;

	explicit GameObject(std::shared_ptr<GameObjectContext> const& context);
	explicit GameObject(std::shared_ptr<GameObjectContext> && context);
	
	GameObject(std::shared_ptr<GameObjectContext> const& context, GameObjectID const& id);
	GameObject(std::shared_ptr<GameObjectContext> && context, GameObjectID const& id);
	
	explicit operator bool() const;
	
	bool operator==(GameObject const& gameObject) const;
	bool operator!=(GameObject const& gameObject) const;
	
	///@~Japanese
	/// @brief インスタンス固有の値である 32 ビットの ID を取得します。
	GameObjectID ID() const;

	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <typename T>
	T const* Component() const;
	
	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <typename T>
	T* Component();
	
	///@~Japanese
	/// @brief 指定されたコンポーネントを持っているかどうか取得します。
	template <typename T>
	bool HasComponent() const;
	
	///@~Japanese
	/// @brief コンポーネントを追加します。
	template <typename T>
	T & AddComponent(std::unique_ptr<T> && component);
	
	///@~Japanese
	/// @brief コンポーネントを追加します。
	template <typename T, typename...Arguments>
	T & AddComponent(Arguments && ...arguments);

	///@~Japanese
	/// @brief 指定されたコンポーネントを削除します。
	template <typename T>
	void RemoveComponent();
	
	///@~Japanese
	/// @brief オブジェクトのコンポーネントをすべて削除し、オブジェクトをゲームワールドから取り除きます。
	void Destroy();
	
private:
	std::shared_ptr<GameObjectContext> context;
	GameObjectID id;
};



template <class T>
class Component;

template <typename T>
T const* GameObject::Component() const
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
	POMDOG_ASSERT(context);
	return context->Component<T>(id);
}

template <typename T>
T* GameObject::Component()
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	POMDOG_ASSERT(context);
	return context->Component<T>(id);
}

template <typename T>
bool GameObject::HasComponent() const
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
	POMDOG_ASSERT(context);
	return context->HasComponent<T>(id);
}

template <typename T>
T & GameObject::AddComponent(std::unique_ptr<T> && component)
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	POMDOG_ASSERT(context);
	return context->AddComponent<T>(id, std::move(component));
}

template <typename T, typename...Arguments>
T & GameObject::AddComponent(Arguments && ...arguments)
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	POMDOG_ASSERT(context);
	return context->AddComponent<T>(id, std::forward<Arguments>(arguments)...);
}

template <typename T>
void GameObject::RemoveComponent()
{
	static_assert(std::is_base_of<Details::Gameplay::GameComponent, T>::value, "");
	POMDOG_ASSERT(context);
	context->RemoveComponent<T>(id);
}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP)
