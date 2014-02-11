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

#include <type_traits>
#include <algorithm>
#include <memory>
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

	explicit GameObject(std::int32_t instanceID);

	GameObject(std::int32_t instanceID, std::size_t minimumCapacity);

	///@~Japanese
	/// @brief インスタンス固有の値である 32 ビットの ID を取得します。
	std::int32_t GetInstanceID() const;

	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <class T>
	T const* GetComponent() const
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});

		if (iter != std::end(components)) {
			POMDOG_ASSERT(*iter);
			if (auto p = dynamic_cast<Details::IntrusiveComponent<T> const*>((*iter).get())) {
				POMDOG_ASSERT(p->Get());
				return p->Get();
			}
		}

		return nullptr;
	}
	
	///@~Japanese
	/// @brief コンポーネントを取得します。
	template <class T>
	T* GetComponent()
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});

		if (iter != std::end(components)) {
			POMDOG_ASSERT(*iter);
			if (auto p = dynamic_cast<Details::IntrusiveComponent<T>*>((*iter).get())) {
				POMDOG_ASSERT(p->Get());
				return p->Get();
			}
		}

		return nullptr;
	}
	
	///@~Japanese
	/// @brief 指定されたコンポーネントを持っているかどうか取得します。
	template <class T>
	bool HasComponent() const
	{
		auto const iter = std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		});

		POMDOG_ASSERT(([&]()->bool {
			if (iter == std::end(components)) {
				return true;
			}
			POMDOG_ASSERT(*iter);
			if (auto p = dynamic_cast<Details::IntrusiveComponent<T> const*>((*iter).get())) {
				POMDOG_ASSERT(p->Get());
				return p->Get() != nullptr;
			}
			return false;
		})());
		return iter != std::end(components);
	}

	///@~Japanese
	/// @brief コンポーネントを追加します。
	template <class T>
	void AddComponent()
	{
		POMDOG_ASSERT(std::end(components) == std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		}));

		components.emplace_back(
			std::unique_ptr<Details::GameComponent>(new Details::IntrusiveComponent<T>));
	}

	///@~Japanese
	/// @brief コンポーネントを追加します。
	template <class T>
	void AddComponent(T const& source)
	{
		POMDOG_ASSERT(std::end(components) == std::find_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				POMDOG_ASSERT(component);
				return component->GetHashCode() == Details::GameComponentTypeID<T>::value;
		}));

		components.emplace_back(
			std::unique_ptr<Details::GameComponent>(new Details::IntrusiveComponent<T>(source)));
	}

	///@~Japanese
	/// @brief 指定されたコンポーネントを削除します。
	template <class T>
	void RemoveComponent()
	{
		components.erase(std::remove_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				auto const hashCode = Details::GameComponentTypeID<T>::value;
				
				POMDOG_ASSERT(component);
				return hashCode == component->GetHashCode();
			}), std::end(components));
	}

private:
	std::vector<std::unique_ptr<Details::GameComponent>> components;
	std::int32_t instanceID;
};

/// @}
/// @}

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECT_D9B3B0C3_2C2C_4EAD_AE7C_374ECF3150C3_HPP)
