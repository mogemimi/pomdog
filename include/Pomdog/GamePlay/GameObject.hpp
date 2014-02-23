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
#include "../Utility/Assert.hpp"
#include "detail/GameComponent.hpp"

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup GamePlay
/// @{

///@~Japanese
/// @brief コンポーネントを保持するゲームオブジェクトです。複数のコンポーネントを格納するコンテナの役割を担います。
class GameObject
{
public:
	GameObject() = default;
	GameObject(GameObject const&) = delete;
	GameObject & operator=(GameObject const&) = delete;
	
	virtual ~GameObject() = default;

	explicit GameObject(std::uint64_t instanceID);

	GameObject(std::uint64_t instanceID, std::size_t minimumCapacity);

	///@~Japanese
	/// @brief インスタンス固有の値である 32 ビットの ID を取得します。
	std::uint64_t InstanceID() const;

	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <typename T>
	T const* Component() const
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});

		if (iter != std::end(components)) {
			POMDOG_ASSERT(*iter);
			POMDOG_ASSERT(HasComponent<T>());
			if (auto p = dynamic_cast<Details::IntrusiveComponent<T> const*>((*iter).get())) {
				return &(p->Value());
			}
		}

		return nullptr;
	}
	
	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <typename T>
	T* Component()
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});

		if (iter != std::end(components)) {
			POMDOG_ASSERT(*iter);
			POMDOG_ASSERT(HasComponent<T>());
			if (auto p = dynamic_cast<Details::IntrusiveComponent<T>*>((*iter).get())) {
				return &(p->Value());
			}
		}

		return nullptr;
	}
	
	///@~Japanese
	/// @brief 指定されたコンポーネントを持っているかどうか取得します。
	template <typename T>
	bool HasComponent() const
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});
		return iter != std::end(components);
	}

	///@~Japanese
	/// @brief コンポーネントを追加します。
	template <typename T, typename...Arguments>
	void AddComponent(Arguments && ...arguments)
	{
		POMDOG_ASSERT(std::end(components) == std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		}));

		components.emplace_back(
			//std::make_unique<Details::IntrusiveComponent<T>>(
			std::unique_ptr<Details::GameComponent>(new Details::IntrusiveComponent<T>(
				std::forward<Arguments>(arguments)...
			)));
	}

	///@~Japanese
	/// @brief 指定されたコンポーネントを削除します。
	template <typename T>
	void RemoveComponent()
	{
		components.erase(std::remove_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return Details::GameComponentTypeID<T>::value == component->GetHashCode();
			}), std::end(components));
	}

private:
	std::vector<std::unique_ptr<Details::GameComponent>> components;
	std::uint64_t instanceID;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP)
