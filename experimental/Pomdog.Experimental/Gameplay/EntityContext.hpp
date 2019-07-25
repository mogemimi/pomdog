// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Component.hpp"
#include "Pomdog.Experimental/Gameplay/EntityID.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <memory>
#include <utility>
#include <vector>
#include <bitset>
#include <list>

namespace Pomdog {
namespace Detail {
namespace Gameplay {

template <std::uint8_t MaxComponentCapacity>
class EntityDescription {
public:
    std::bitset<MaxComponentCapacity> ComponentBitMask;
    std::uint32_t IncremantalCounter = 1;
};

template <std::uint8_t MaxComponentCapacity>
class EntityContext {
public:
    EntityContext();

    EntityID Create(
        std::vector<std::shared_ptr<ComponentCreatorBase>> && componentCreators);

    std::size_t GetCount() const noexcept;

    std::size_t GetCapacity() const noexcept;

    void Clear();

    void Refresh();

    void Destroy(const EntityID& id);

    void DestroyImmediate(const EntityID& id);

    bool Valid(const EntityID& id) const noexcept;

    template <typename Type>
    bool HasComponent(const EntityID& id) const;

    template <typename T, typename...Components>
    bool HasComponents(const EntityID& id) const;

    template <typename Type>
    auto GetComponent(const EntityID& id)
        -> std::enable_if_t<
            std::is_base_of<Component, Type>::value,
            std::shared_ptr<Type>>;

    template <typename Type>
    auto GetComponent(const EntityID& id)
        -> std::enable_if_t<
            !std::is_base_of<Component, Type>::value,
            std::shared_ptr<Type>>;

private:
    void AddComponent(
        const EntityID& entityId,
        std::uint8_t componentTypeIndex,
        std::shared_ptr<Component> && component);

    void DestroyComponents(std::uint32_t index);

private:
    std::vector<std::vector<std::shared_ptr<Component>>> components;
    std::vector<EntityDescription<MaxComponentCapacity>> descriptions;
    std::list<std::uint32_t> deletedIndices;
    std::list<EntityID> destroyedObjects;
    std::size_t entityCount;
};

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
bool EntityContext<MaxComponentCapacity>::HasComponent(const EntityID& id) const
{
    static_assert(std::is_base_of<Component, Type>::value, "");
    const auto typeIndex = ComponentTypeDeclaration<Type>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
    POMDOG_ASSERT(id.Index() < descriptions.size());
    return descriptions[id.Index()].ComponentBitMask[typeIndex];
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
    static_assert(std::is_base_of<Component, Type>::value, "");
    const auto typeIndex = ComponentTypeDeclaration<Type>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    auto mask = ComponentMask<MaxComponentCapacity, Components...>();
    mask[typeIndex] = true;
    return mask;
}

} // namespace Helper

template <std::uint8_t MaxComponentCapacity>
template <typename Type, typename...Components>
bool EntityContext<MaxComponentCapacity>::HasComponents(const EntityID& id) const
{
    static_assert(std::is_base_of<Component, Type>::value, "");
    const auto mask = Helper::ComponentMask<MaxComponentCapacity, Type, Components...>();
    POMDOG_ASSERT(id.Index() < descriptions.size());
    return (descriptions[id.Index()].ComponentBitMask & mask) == mask;
}

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
auto EntityContext<MaxComponentCapacity>::GetComponent(const EntityID& id)
    -> std::enable_if_t<
        std::is_base_of<Component, Type>::value,
        std::shared_ptr<Type>>
{
    static_assert(std::is_base_of<Component, Type>::value, "");

    const auto typeIndex = ComponentTypeDeclaration<Type>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    if (typeIndex >= components.size()) {
        return nullptr;
    }

    auto & entities = components[typeIndex];

    if (id.Index() >= entities.size()) {
        return nullptr;
    }

    POMDOG_ASSERT(id.Index() < entities.size());

    if (entities[id.Index()]) {
        POMDOG_ASSERT(id.Index() < descriptions.size());
        POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[typeIndex]);
        POMDOG_ASSERT(
            dynamic_cast<Type*>(entities[id.Index()].get()) ==
            static_cast<Type*>(entities[id.Index()].get()));
        return std::static_pointer_cast<Type>(entities[id.Index()]);
    }
    return nullptr;
}

template <std::uint8_t MaxComponentCapacity>
template <typename Type>
auto EntityContext<MaxComponentCapacity>::GetComponent(const EntityID& id)
    -> std::enable_if_t<
        !std::is_base_of<Component, Type>::value,
        std::shared_ptr<Type>>
{
    static_assert(std::is_base_of<Component, Type>::value, "");

    const auto typeIndex = ComponentTypeDeclaration<Type>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    if (typeIndex >= components.size()) {
        return nullptr;
    }

    auto & entities = components[typeIndex];

    if (id.Index() >= entities.size()) {
        return nullptr;
    }

    POMDOG_ASSERT(id.Index() < entities.size());

    if (entities[id.Index()]) {
        POMDOG_ASSERT(id.Index() < descriptions.size());
        POMDOG_ASSERT(descriptions[id.Index()].ComponentBitMask[typeIndex]);
        return std::dynamic_pointer_cast<Type>(entities[id.Index()]);
    }
    return nullptr;
}

} // namespace Gameplay
} // namespace Detail

using EntityContext = Detail::Gameplay::EntityContext<64>;

} // namespace Pomdog
