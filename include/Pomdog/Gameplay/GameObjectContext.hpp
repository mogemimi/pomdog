//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GAMEOBJECTCONTEXT_DFD4E6A6_66AE_4494_85C3_7155EF07CD89_HPP
#define POMDOG_GAMEOBJECTCONTEXT_DFD4E6A6_66AE_4494_85C3_7155EF07CD89_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include <list>
#include "../Config/Export.hpp"
#include "../Utility/Assert.hpp"
#include "detail/GameComponent.hpp"
#include "GameObjectID.hpp"

namespace Pomdog {
namespace Details {
namespace Gameplay {

template <std::uint8_t MaxComponentCapacity>
class POMDOG_EXPORT EntityDescription {
public:
	std::bitset<MaxComponentCapacity> ComponentBitMask;
	std::uint32_t IncremantalCounter = 0;
};

template <std::uint8_t MaxComponentCapacity>
class POMDOG_EXPORT EntityContext {
private:
	typedef std::uint8_t HashCodeType;
	typedef GameComponent<HashCodeType> ComponentType;
	
	template <typename T>
	using ComponentHash = GameComponentHashCode<T, HashCodeType>;
	
	template <typename T>
	using IntrusiveComponentType = IntrusiveComponent<T, HashCodeType>;

public:
	EntityContext()
	{
		static_assert(MaxComponentCapacity > 0, "");
		components.reserve(MaxComponentCapacity);
	}

	GameObjectID Create()
	{
		std::uint32_t index = 0;
		if (deletedIndices.empty())
		{
			POMDOG_ASSERT(std::numeric_limits<decltype(index)>::max() > descriptions.size());
			index = static_cast<std::uint32_t>(descriptions.size());
			descriptions.resize(descriptions.size() + 1);
		}
		else
		{
			index = deletedIndices.front();
			deletedIndices.pop_front();
		}
		
		auto & desc = descriptions[index];
		
		desc.ComponentBitMask.reset();
		++desc.IncremantalCounter;

		POMDOG_ASSERT(desc.IncremantalCounter > 0);
		
		#ifdef DEBUG
		{
			for (auto & entities: components)
			{
				if (index < entities.size()) {
					POMDOG_ASSERT(!entities[index]);
				}
			}
		}
		#endif
		
		return {desc.IncremantalCounter, index};
	}
	
	void Destroy(GameObjectID const& objectID)
	{
		///@todo remove components
		for (auto & entities: components)
		{
			if (objectID.Index() < entities.size())
			{
				entities[objectID.Index()].reset();
			}
		}
		
		POMDOG_ASSERT(objectID.Index() < descriptions.size());
		POMDOG_ASSERT(descriptions[objectID.Index()].IncremantalCounter == objectID.SequenceNumber());
		
		descriptions[objectID.Index()].ComponentBitMask.reset();
		
		deletedIndices.push_back(objectID.Index());
	}
	
	bool Valid(GameObjectID const& objectID) const
	{
		return (objectID.Index() < descriptions.size())
			&& (descriptions[objectID.Index()].IncremantalCounter == objectID.SequenceNumber());
	}
	
	template <typename T, typename...Arguments>
	T* AddComponent(GameObjectID const& objectID, Arguments &&...arguments)
	{
		//auto component = std::make_unique<IntrusiveComponentType<T>>(
		auto component = std::unique_ptr<IntrusiveComponentType<T>>(new IntrusiveComponentType<T>(
			std::forward<Arguments>(arguments)...));
		
		POMDOG_ASSERT(component);
		POMDOG_ASSERT(component->GetHashCode() < MaxComponentCapacity);

		auto const hashCode = component->GetHashCode();
		
		if (hashCode >= components.size())
		{
			components.resize(hashCode + 1U);
		
			POMDOG_ASSERT(components.size() <= MaxComponentCapacity);
			if (components.capacity() > MaxComponentCapacity)
			{
				components.shrink_to_fit();
				POMDOG_ASSERT(components.capacity() == MaxComponentCapacity);
			}
		}
		
		POMDOG_ASSERT(hashCode < components.size());
		auto & entities = components[hashCode];
		
		if (objectID.Index() >= entities.size())
		{
			static_assert(std::is_unsigned<decltype(objectID.Index())>::value, "" );
			entities.resize(objectID.Index() + 1U);
		}
		
		POMDOG_ASSERT(objectID.Index() < entities.size());
		entities[objectID.Index()] = std::move(component);
		
		POMDOG_ASSERT(objectID.Index() < descriptions.size());
		auto & desc = descriptions[objectID.Index()];
		
		POMDOG_ASSERT(desc.IncremantalCounter > 0);
		POMDOG_ASSERT(hashCode < desc.ComponentBitMask.size());
		POMDOG_ASSERT(hashCode < MaxComponentCapacity);
		desc.ComponentBitMask[hashCode] = 1;
		
		auto result = entities[objectID.Index()].get();
		
		POMDOG_ASSERT(result != nullptr);
		POMDOG_ASSERT(dynamic_cast<IntrusiveComponentType<T>*>(result) == static_cast<IntrusiveComponentType<T>*>(result));
		return &(static_cast<IntrusiveComponentType<T>*>(result)->Value());
	}
	
	template <typename T>
	void RemoveComponent(GameObjectID const& id)
	{
		POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
		POMDOG_ASSERT(id.Index() < descriptions.size());
	
		if (ComponentHash<T>::value >= components.size()) {
			return;
		}
		
		auto & entities = components[ComponentHash<T>::value];
		
		POMDOG_ASSERT(!entities.empty());
		POMDOG_ASSERT(id.Index() < entities.size());
		entities[id.Index()].reset();
		
		POMDOG_ASSERT(id.Index() < descriptions.size());
		auto & desc = descriptions[id.Index()];
		
		POMDOG_ASSERT(ComponentHash<T>::value < desc.ComponentBitMask.size());
		desc.ComponentBitMask[ComponentHash<T>::value] = 0;
	}
	
	template <typename T>
	bool HasComponent(GameObjectID const& id) const
	{
		POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
		POMDOG_ASSERT(id.Index() < descriptions.size());
		return descriptions[id.Index()].ComponentBitMask[ComponentHash<T>::value];
	}
	
	template <typename T>
	T const* Component(GameObjectID const& id) const
	{
		POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
	
		if (ComponentHash<T>::value >= components.size()) {
			return nullptr;
		}
		
		auto & entities = components[ComponentHash<T>::value];
		
		POMDOG_ASSERT(id.Index() < entities.size());
		if (auto component = static_cast<IntrusiveComponentType<T> const*>(entities[id.Index()].get()))
		{
			POMDOG_ASSERT(id.Index() < descriptions.size());
			POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[ComponentHash<T>::value]);
			POMDOG_ASSERT(nullptr != component);
			return &(component->Value());
		}
		return nullptr;
	}
	
	template <typename T>
	T* Component(GameObjectID const& id)
	{
		POMDOG_ASSERT(ComponentHash<T>::value < MaxComponentCapacity);
	
		if (ComponentHash<T>::value >= components.size()) {
			return nullptr;
		}
		
		auto & entities = components[ComponentHash<T>::value];
		
		if (id.Index() >= entities.size()) {
			return nullptr;
		}
		
		POMDOG_ASSERT(id.Index() < entities.size());
		
		if (auto component = static_cast<IntrusiveComponentType<T>*>(entities[id.Index()].get()))
		{
			POMDOG_ASSERT(nullptr != component);
			POMDOG_ASSERT(id.Index() < descriptions.size());
			POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[ComponentHash<T>::value]);
			return &(component->Value());
		}
		return nullptr;
	}

private:
	std::vector<std::vector<std::unique_ptr<ComponentType>>> components;
	std::vector<EntityDescription<MaxComponentCapacity>> descriptions;
	std::list<std::uint32_t> deletedIndices;
};

}// namespace Gameplay
}// namespace Details

using GameObjectContext = Details::Gameplay::EntityContext<96U>;

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECTCONTEXT_DFD4E6A6_66AE_4494_85C3_7155EF07CD89_HPP)
