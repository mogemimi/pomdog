//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEOBJECT_H
#define POMDOG_GAMEOBJECT_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/GameComponent.hpp"
#include "../Utility/Assert.hpp"
#include "../Utility/Noncopyable.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>

namespace Pomdog {

/// @addtogroup Framework
/// @{
/// @addtogroup GamePlay
/// @{

///@~Japanese
/// @brief コンポーネントを保持するハブの役割を担うゲームオブジェクトです。
/// @remarks ビデオゲームに出てくるオブジェクトは、それぞれ固有の情報を持っています。
/// 例えば、現在の位置や、衝突判定用のモデル、レンダリングで用いるマテリアル情報、ヒットポイント(health) など様々です。
/// これらを管理するために GameObject インターフェイスがあるとします。
/// GameObject を継承して次のようなクラスを定義したとします：
/// * 位置座標などを持つ MovableObject クラス
/// * 物理で用いる PhysicsObject クラス
/// * レンダリングで使用する RenderableObject クラス
///
/// このような設計では GameObject を継承してそれぞれ、陸地を走る Vehicle クラス、
/// 水辺を走る Boat クラスを作ることになります。
/// ここで、水陸両用の乗り物を定義するにはどうすればよいでしょうか。
/// Vehicle と Boat を多重継承するべきでしょうか。
///
/// これらの問題を解決するために、コンポーネントの概念を扱います。
/// また GameObject はコンポーネントを保持するハブ、またはコンテナと考えることができます。
/// GameComponent インターフェイスを継承し、陸地を走るために必要な情報を VehicleComponent と定義し、
/// 水辺を走るために必要な情報を BoatComponent と定義します。
/// 水陸両用の乗り物を定義する場合は、GameObject に VehicleComponent と BoatComponent を追加します。
///
/// コンポーネントを用いれば MovableObject といったインターフェイスを定義する必要はありません。
/// GameEntity に Transform コンポーネントを追加するだけで済みます。
/// 最初に例をあげた設計は、is-a の関係を使っています。
/// それに対し、コンポーネントとコンポーネントのハブとなるエンティティの場合 has-a の関係になります。
/// 疎結合のため、より柔軟なゲーム内オブジェクトを定義することが可能です。
class GameObject: Noncopyable
{
public:
	GameObject() = default;
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
				return component->GetHashCode() == Details::ComponentTypeID<T>::value;
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
				return component->GetHashCode() == Details::ComponentTypeID<T>::value;
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
				return component->GetHashCode() == Details::ComponentTypeID<T>::value;
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
				return component->GetHashCode() == Details::ComponentTypeID<T>::value;
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
				return component->GetHashCode() == Details::ComponentTypeID<T>::value;
		}));

		components.emplace_back(
			std::unique_ptr<Details::GameComponent>(new Details::IntrusiveComponent<T>(source)));
	}

	///@~Japanese
	///@brief 指定されたコンポーネントを削除します。
	template <class T>
	void RemoveComponent()
	{
		components.erase(std::remove_if(std::begin(components), std::end(components),
			[](std::unique_ptr<Details::GameComponent> const& component) {
				auto const hashCode = Details::ComponentTypeID<T>::value;
				
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

#endif // !defined(POMDOG_GAMEOBJECT_H)
