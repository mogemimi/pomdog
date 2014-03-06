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

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include "../Config/Export.hpp"
#include "../Utility/Assert.hpp"
#include "detail/GameComponent.hpp"
#include "GameObjectID.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup Gameplay
/// @{

///@~Japanese
/// @brief コンポーネントを保持するゲームオブジェクトです。複数のコンポーネントを格納するコンテナの役割を担います。
class POMDOG_EXPORT GameObject
{
private:
	typedef std::uint8_t HashCodeType;
	typedef Details::GameComponent<HashCodeType> ComponentType;
	
	template <typename T>
	using ComponentHash = Details::GameComponentHashCode<T, HashCodeType>;
	
	template <typename T>
	using IntrusiveComponentType = Details::IntrusiveComponent<T, HashCodeType>;
	
	static constexpr HashCodeType MaxComponentCapacity = 96;

public:
	GameObject() = default;
	GameObject(GameObject const&) = delete;
	GameObject & operator=(GameObject const&) = delete;
	
	virtual ~GameObject() = default;

	explicit GameObject(GameObjectID const& instanceID);
	explicit GameObject(GameObjectID && instanceID);

	///@~Japanese
	/// @brief インスタンス固有の値である 32 ビットの ID を取得します。
	GameObjectID const& ID() const;

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
	template <typename T, typename...Arguments>
	T* AddComponent(Arguments && ...arguments);

	///@~Japanese
	/// @brief 指定されたコンポーネントを削除します。
	template <typename T>
	void RemoveComponent();
	
private:
	std::bitset<MaxComponentCapacity> componentBitMask;
	std::vector<std::unique_ptr<ComponentType>> components;
	GameObjectID instanceID;
};

/// @}
/// @}


template <typename T>
T const* GameObject::Component() const
{
	auto const iter = std::find_if(std::begin(components), std::end(components),
		[](std::unique_ptr<ComponentType> const& component) {
			POMDOG_ASSERT(component);
			return component->GetHashCode() == ComponentHash<T>::value;
	});

	if (iter != std::end(components))
	{
		POMDOG_ASSERT(*iter);
		POMDOG_ASSERT(HasComponent<T>());
		if (auto p = dynamic_cast<IntrusiveComponentType<T> const*>((*iter).get()))
		{
			return &(p->Value());
		}
	}
	return nullptr;
}

template <typename T>
T* GameObject::Component()
{
	auto const iter = std::find_if(std::begin(components), std::end(components),
		[](std::unique_ptr<ComponentType> const& component) {
			POMDOG_ASSERT(component);
			return component->GetHashCode() == ComponentHash<T>::value;
	});

	if (iter != std::end(components))
	{
		POMDOG_ASSERT(*iter);
		POMDOG_ASSERT(HasComponent<T>());
		if (auto p = dynamic_cast<IntrusiveComponentType<T>*>((*iter).get()))
		{
			return &(p->Value());
		}
	}
	return nullptr;
}

template <typename T>
bool GameObject::HasComponent() const
{
	POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
	return componentBitMask[ComponentHash<T>::value];
}

template <typename T, typename...Arguments>
T* GameObject::AddComponent(Arguments && ...arguments)
{
	POMDOG_ASSERT(components.size() < MaxComponentCapacity);
	
	auto const hashCode = ComponentHash<T>::value;

	POMDOG_ASSERT(std::end(components) == std::find_if(std::begin(components), std::end(components),
		[&](std::unique_ptr<ComponentType> const& component) { return component->GetHashCode() == hashCode; }));

	components.push_back(
		//std::make_unique<IntrusiveComponentType<T>>(
		std::unique_ptr<IntrusiveComponentType<T>>(new IntrusiveComponentType<T>(
			std::forward<Arguments>(arguments)...
		)));
	
	POMDOG_ASSERT(hashCode < MaxComponentCapacity);
	POMDOG_ASSERT(!componentBitMask[hashCode]);
	componentBitMask[hashCode] = 1;
	
	return &(dynamic_cast<IntrusiveComponentType<T>*>((components.back()).get())->Value());
}

template <typename T>
void GameObject::RemoveComponent()
{
	components.erase(std::remove_if(std::begin(components), std::end(components),
		[](std::unique_ptr<ComponentType> const& component) {
			POMDOG_ASSERT(component);
			return component->GetHashCode() == ComponentHash<T>::value;
		}), std::end(components));

	POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
	componentBitMask[ComponentHash<T>::value] = 0;
}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP)
