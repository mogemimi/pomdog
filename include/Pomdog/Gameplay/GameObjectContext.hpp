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

#include <cstdint>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include <list>
#include "../Config/Export.hpp"
#include "../Utility/Assert.hpp"
#include "Component.hpp"
#include "GameObjectID.hpp"

namespace Pomdog {
namespace Details {
namespace Gameplay {

template <std::uint8_t MaxComponentCapacity>
class POMDOG_EXPORT EntityDescription {
public:
	std::bitset<MaxComponentCapacity> ComponentBitMask;
	std::uint32_t IncremantalCounter = 1;
};

template <std::uint8_t MaxComponentCapacity>
class POMDOG_EXPORT EntityContext {
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
		POMDOG_ASSERT(desc.ComponentBitMask.none());
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
	
	void Destroy(GameObjectID const& id)
	{
		POMDOG_ASSERT(Valid(id));

		auto const index = id.Index();

		///@todo remove components
		for (auto & entities: components)
		{
			if (index < entities.size())
			{
				entities[index].reset();
			}
		}
		
		POMDOG_ASSERT(index < descriptions.size());
		POMDOG_ASSERT(descriptions[index].IncremantalCounter == id.SequenceNumber());
		
		auto & desc = descriptions[index];
		desc.ComponentBitMask.reset();
		++desc.IncremantalCounter;
		
		deletedIndices.push_back(index);
	}
	
	bool Valid(GameObjectID const& id) const
	{
		return (id.Index() < descriptions.size())
			&& (descriptions[id.Index()].IncremantalCounter == id.SequenceNumber());
	}
	
	template <typename Type, typename...Arguments>
	Type & AddComponent(GameObjectID const& id, Arguments &&...arguments)
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");
		POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);

		auto component = std::make_unique<Type>(std::forward<Arguments>(arguments)...);
		return AddComponent<Type>(id, std::move(component));
	}
	
	template <typename Type>
	Type & AddComponent(GameObjectID const& id, std::unique_ptr<Type> && component)
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");

		auto const typeIndex = Type::TypeIndex();
		POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
		if (typeIndex >= components.size())
		{
			components.resize(typeIndex + 1U);
		
			POMDOG_ASSERT(components.size() <= MaxComponentCapacity);
			if (components.capacity() > MaxComponentCapacity)
			{
				components.shrink_to_fit();
				POMDOG_ASSERT(components.capacity() == MaxComponentCapacity);
			}
		}
		
		POMDOG_ASSERT(typeIndex < components.size());
		auto & entities = components[typeIndex];
		
		if (id.Index() >= entities.size())
		{
			static_assert(std::is_unsigned<decltype(id.Index())>::value, "" );
			entities.resize(id.Index() + 1U);
		}
	
		POMDOG_ASSERT(component);
		POMDOG_ASSERT(id.Index() < entities.size());
		entities[id.Index()] = std::move(component);

		POMDOG_ASSERT(id.Index() < descriptions.size());
		auto & desc = descriptions[id.Index()];
		
		POMDOG_ASSERT(desc.IncremantalCounter > 0);
		POMDOG_ASSERT(typeIndex < desc.ComponentBitMask.size());
		POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
		desc.ComponentBitMask[typeIndex] = 1;
		
		POMDOG_ASSERT(entities[id.Index()]);
		POMDOG_ASSERT(entities[id.Index()].get() != nullptr);
		POMDOG_ASSERT(dynamic_cast<Type*>(entities[id.Index()].get()) == static_cast<Type*>(entities[id.Index()].get()));
		return *static_cast<Type*>(entities[id.Index()].get());
	}
	
	template <typename Type>
	void RemoveComponent(GameObjectID const& id)
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");
		
		auto const typeIndex = Type::TypeIndex();
		POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
		POMDOG_ASSERT(id.Index() < descriptions.size());
	
		if (typeIndex >= components.size()) {
			return;
		}
		
		auto & entities = components[typeIndex];
		
		POMDOG_ASSERT(!entities.empty());
		POMDOG_ASSERT(id.Index() < entities.size());
		entities[id.Index()].reset();
		
		POMDOG_ASSERT(id.Index() < descriptions.size());
		auto & desc = descriptions[id.Index()];
		
		POMDOG_ASSERT(typeIndex < desc.ComponentBitMask.size());
		desc.ComponentBitMask[typeIndex] = 0;
	}
	
	template <typename Type>
	bool HasComponent(GameObjectID const& id) const
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");
		
		POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);
		POMDOG_ASSERT(id.Index() < descriptions.size());
		return descriptions[id.Index()].ComponentBitMask[Type::TypeIndex()];
	}
		
	template <typename Type>
	auto Component(GameObjectID const& id)-> typename std::enable_if<std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");
		static_assert(std::is_base_of<Pomdog::Component<Type>, Type>::value, "");
	
		auto const typeIndex = Type::TypeIndex();
		POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
	
		if (typeIndex >= components.size()) {
			return nullptr;
		}
		
		auto & entities = components[typeIndex];
		
		if (id.Index() >= entities.size()) {
			return nullptr;
		}
		
		POMDOG_ASSERT(id.Index() < entities.size());

		if (entities[id.Index()])
		{
			POMDOG_ASSERT(id.Index() < descriptions.size());
			POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[typeIndex]);
			POMDOG_ASSERT(dynamic_cast<Type*>(entities[id.Index()].get()) == static_cast<Type*>(entities[id.Index()].get()));
			return static_cast<Type*>(entities[id.Index()].get());
		}
		return nullptr;
	}
	
	template <typename Type>
	auto Component(GameObjectID const& id)-> typename std::enable_if<!std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type
	{
		static_assert(std::is_base_of<GameComponent, Type>::value, "");
		static_assert(!std::is_base_of<Pomdog::Component<Type>, Type>::value, "");
	
		auto const typeIndex = Type::TypeIndex();
		POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
	
		if (typeIndex >= components.size()) {
			return nullptr;
		}
		
		auto & entities = components[typeIndex];
		
		if (id.Index() >= entities.size()) {
			return nullptr;
		}
		
		POMDOG_ASSERT(id.Index() < entities.size());

		if (entities[id.Index()])
		{
			POMDOG_ASSERT(id.Index() < descriptions.size());
			POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[typeIndex]);
			return dynamic_cast<Type*>(entities[id.Index()].get());
		}
		return nullptr;
	}

private:
	std::vector<std::vector<std::unique_ptr<GameComponent>>> components;
	std::vector<EntityDescription<MaxComponentCapacity>> descriptions;
	std::list<std::uint32_t> deletedIndices;
};

}// namespace Gameplay
}// namespace Details

using GameObjectContext = Details::Gameplay::EntityContext<128U>;

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECTCONTEXT_DFD4E6A6_66AE_4494_85C3_7155EF07CD89_HPP)
