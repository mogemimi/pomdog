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
#pragma once
#endif

#include "Component.hpp"
#include "GameObjectID.hpp"
#include "Pomdog/Config/Export.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include <list>

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
	EntityContext();

	GameObjectID Create();
	
	std::size_t Count() const;
	
	std::size_t Capacity() const;

	void Clear();

	void Refresh();
	
	void Destroy(GameObjectID const& id);
	
	void DestroyImmediate(GameObjectID const& id);
	
	bool Valid(GameObjectID const& id) const;
	
	template <typename Type, typename...Arguments>
	Type & AddComponent(GameObjectID const& id, Arguments &&...arguments);
	
	template <typename Type>
	Type & AddComponent(GameObjectID const& id, std::unique_ptr<Type> && component);
	
	template <typename Type>
	void RemoveComponent(GameObjectID const& id);
	
	template <typename Type>
	bool HasComponent(GameObjectID const& id) const;
	
	template <typename T, typename...Components>
	bool HasComponents(GameObjectID const& id) const;
	
	template <typename Type>
	auto Component(GameObjectID const& id)-> typename std::enable_if<std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type;
	
	template <typename Type>
	auto Component(GameObjectID const& id)-> typename std::enable_if<!std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type;

private:
	void DestroyComponents(std::uint32_t index);

private:
	std::vector<std::vector<std::unique_ptr<GameComponent>>> components;
	std::vector<EntityDescription<MaxComponentCapacity>> descriptions;
	std::list<std::uint32_t> deletedIndices;
	std::list<GameObjectID> destroyedObjects;
	std::size_t entityCount;
};


template <std::uint8_t MaxComponentCapacity>
EntityContext<MaxComponentCapacity>::EntityContext()
	: entityCount(0)
{
	static_assert(MaxComponentCapacity > 0, "");
	components.reserve(MaxComponentCapacity);
}

template <std::uint8_t MaxComponentCapacity>
GameObjectID EntityContext<MaxComponentCapacity>::Create()
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
	
	++entityCount;
	return {desc.IncremantalCounter, index};
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityContext<MaxComponentCapacity>::Count() const
{
	return entityCount;
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityContext<MaxComponentCapacity>::Capacity() const
{
	return descriptions.capacity();
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Clear()
{
	for (std::uint32_t index = 0; index < descriptions.size(); ++index)
	{
		auto & desc = descriptions[index];
		
		if (desc.ComponentBitMask.any()) {
			DestroyComponents(index);
		}
		deletedIndices.push_back(index);
		
		desc.ComponentBitMask.reset();
		++desc.IncremantalCounter;
	#ifdef DEBUG
		POMDOG_ASSERT(entityCount > 0);
		--entityCount;
	#endif
	}
#ifdef DEBUG
	POMDOG_ASSERT(entityCount == 0);
#endif
	entityCount = 0;
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::DestroyComponents(std::uint32_t index)
{
	for (auto & entities: components)
	{
		if (index < entities.size())
		{
			entities[index].reset();
		}
	}
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Refresh()
{
	for (auto & id: destroyedObjects)
	{
		POMDOG_ASSERT(!Valid(id));
		auto index = id.Index();
		DestroyComponents(index);
		deletedIndices.push_back(index);
	}
	
	destroyedObjects.clear();
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Destroy(GameObjectID const& id)
{
	POMDOG_ASSERT(Valid(id));
	
	auto const index = id.Index();
	
	POMDOG_ASSERT(index < descriptions.size());
	POMDOG_ASSERT(descriptions[index].IncremantalCounter == id.SequenceNumber());
	
	auto & desc = descriptions[index];
	desc.ComponentBitMask.reset();
	++desc.IncremantalCounter;
	
	destroyedObjects.push_back(id);
	
	POMDOG_ASSERT(entityCount > 0);
	--entityCount;
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::DestroyImmediate(GameObjectID const& id)
{
	POMDOG_ASSERT(Valid(id));

	auto const index = id.Index();

	POMDOG_ASSERT(index < descriptions.size());
	POMDOG_ASSERT(descriptions[index].IncremantalCounter == id.SequenceNumber());
	
	auto & desc = descriptions[index];
	desc.ComponentBitMask.reset();
	++desc.IncremantalCounter;
	
	DestroyComponents(index);
	deletedIndices.push_back(index);
	
	POMDOG_ASSERT(entityCount > 0);
	--entityCount;
}

template <std::uint8_t MaxComponentCapacity>
bool EntityContext<MaxComponentCapacity>::Valid(GameObjectID const& id) const
{
	return (id.Index() < descriptions.size())
		&& (descriptions[id.Index()].IncremantalCounter == id.SequenceNumber());
}

template <std::uint8_t MaxComponentCapacity>
template <typename Type, typename...Arguments>
Type & EntityContext<MaxComponentCapacity>::AddComponent(GameObjectID const& id, Arguments &&...arguments)
{
	static_assert(std::is_base_of<GameComponent, Type>::value, "");
	POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);

	auto component = std::make_unique<Type>(std::forward<Arguments>(arguments)...);
	return AddComponent<Type>(id, std::move(component));
}

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
Type & EntityContext<MaxComponentCapacity>::AddComponent(GameObjectID const& id, std::unique_ptr<Type> && component)
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

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
void EntityContext<MaxComponentCapacity>::RemoveComponent(GameObjectID const& id)
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

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
bool EntityContext<MaxComponentCapacity>::HasComponent(GameObjectID const& id) const
{
	static_assert(std::is_base_of<GameComponent, Type>::value, "");
	static_assert(std::is_base_of<Pomdog::Component<Type>, Type>::value, "TOOD: Not implemented");
	
	POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);
	POMDOG_ASSERT(id.Index() < descriptions.size());
	return descriptions[id.Index()].ComponentBitMask[Type::TypeIndex()];
}

namespace Helper {

template <std::uint8_t MaxComponentCapacity>
std::bitset<MaxComponentCapacity> ComponentMask()
{
	return std::bitset<MaxComponentCapacity>{};
}

template <std::uint8_t MaxComponentCapacity, typename Type, typename...Components>
std::bitset<MaxComponentCapacity> ComponentMask()
{
	static_assert(std::is_base_of<GameComponent, Type>::value, "");
	static_assert(std::is_base_of<Component<Type>, Type>::value, "TOOD: Not implemented");
	auto mask = ComponentMask<MaxComponentCapacity, Components...>();
	POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);
	mask[Type::TypeIndex()] = true;
	return std::move(mask);
}

}// namespace Helper

template <std::uint8_t MaxComponentCapacity>
template <typename Type, typename...Components>
bool EntityContext<MaxComponentCapacity>::HasComponents(GameObjectID const& id) const
{
	static_assert(std::is_base_of<GameComponent, Type>::value, "");
	static_assert(std::is_base_of<Pomdog::Component<Type>, Type>::value, "TOOD: Not implemented");
	
	POMDOG_ASSERT(Type::TypeIndex() < MaxComponentCapacity);
	POMDOG_ASSERT(id.Index() < descriptions.size());
	auto mask = Helper::ComponentMask<MaxComponentCapacity, Type, Components...>();
	return (descriptions[id.Index()].ComponentBitMask & mask) == mask;
}

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
auto EntityContext<MaxComponentCapacity>::Component(GameObjectID const& id)
	-> typename std::enable_if<std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type
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

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
auto EntityContext<MaxComponentCapacity>::Component(GameObjectID const& id)
	-> typename std::enable_if<!std::is_base_of<Pomdog::Component<Type>, Type>::value, Type*>::type
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

}// namespace Gameplay
}// namespace Details

using GameObjectContext = Details::Gameplay::EntityContext<64>;

}// namespace Pomdog

#endif // !defined(POMDOG_GAMEOBJECTCONTEXT_DFD4E6A6_66AE_4494_85C3_7155EF07CD89_HPP)
