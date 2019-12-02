// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/ECS/ComponentBuffer.hpp"
#include "Pomdog/Experimental/ECS/ComponentType.hpp"
#include "Pomdog/Experimental/ECS/Entity.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <bitset>
#include <cstdint>
#include <deque>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace Pomdog::ECS::Detail {

template <std::uint8_t MaxComponentCapacity>
class EntityDescription final {
public:
    std::bitset<MaxComponentCapacity> ComponentBitMask;
    std::uint32_t IncremantalVersion = 1;
    bool IsEnabled = false;
};

template <std::uint8_t MaxComponentCapacity>
class EntityManager final {
public:
    EntityManager();

    [[nodiscard]] Entity
    CreateEntity(std::vector<std::shared_ptr<ComponentTypeBase>>&& componentTypes);

    void DestroyEntity(const Entity& entity);

    void DestroyAllEntities();

    bool Exists(const Entity& entity) const noexcept;

    template <typename T>
    bool HasComponent(const Entity& entity) const;

    template <typename T, typename... Components>
    bool HasComponents(const Entity& entity) const;

    template <typename T>
    T* GetComponent(const Entity& entity);

    template <typename T>
    void SetComponentData(const Entity& entity, T&& data);

    template <typename T, typename... Components>
    std::vector<Entity> QueryComponents();

    std::size_t GetCount() const noexcept;

    std::size_t GetCapacity() const noexcept;

private:
    void AddComponent(const Entity& entity, ComponentTypeBase& componentType);

private:
    std::vector<std::unique_ptr<ComponentBufferBase>> components;
    std::vector<EntityDescription<MaxComponentCapacity>> descriptions;
    std::deque<std::uint32_t> deletedIndices;
    std::size_t entityCount;
};

template <std::uint8_t MaxComponentCapacity>
template <typename T>
bool EntityManager<MaxComponentCapacity>::HasComponent(const Entity& entity) const
{
    const auto typeIndex = ComponentTypeDeclaration<T>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
    POMDOG_ASSERT(entity.GetIndex() < descriptions.size());
    return descriptions[entity.GetIndex()].ComponentBitMask[typeIndex];
}

namespace Helper {

template <std::uint8_t MaxComponentCapacity>
std::bitset<MaxComponentCapacity> ComponentMask()
{
    return std::bitset<MaxComponentCapacity>{};
}

template <std::uint8_t MaxComponentCapacity, typename Type, typename... Components>
std::bitset<MaxComponentCapacity> ComponentMask()
{
    const auto typeIndex = ComponentTypeDeclaration<Type>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    auto mask = ComponentMask<MaxComponentCapacity, Components...>();
    mask[typeIndex] = true;
    return mask;
}

} // namespace Helper

template <std::uint8_t MaxComponentCapacity>
template <typename Type, typename... Components>
bool EntityManager<MaxComponentCapacity>::HasComponents(const Entity& entity) const
{
    const auto mask = Helper::ComponentMask<MaxComponentCapacity, Type, Components...>();
    POMDOG_ASSERT(entity.GetIndex() < descriptions.size());
    return (descriptions[entity.GetIndex()].ComponentBitMask & mask) == mask;
}

template <std::uint8_t MaxComponentCapacity>
template <typename T>
T* EntityManager<MaxComponentCapacity>::GetComponent(const Entity& entity)
{
    const auto typeIndex = ComponentTypeDeclaration<T>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    if (typeIndex >= components.size()) {
        return nullptr;
    }

    auto& entities = components[typeIndex];
    if (entities == nullptr) {
        return nullptr;
    }

    POMDOG_ASSERT(entity.GetIndex() < descriptions.size());
    auto& desc = descriptions[entity.GetIndex()];
    if (desc.IncremantalVersion != entity.GetVersion()) {
        return nullptr;
    }

    POMDOG_ASSERT(entities != nullptr);
    if (entity.GetIndex() >= entities->GetSize()) {
        return nullptr;
    }

    POMDOG_ASSERT(entity.GetIndex() < entities->GetSize());

    auto nativeEntities = static_cast<Detail::ComponentBuffer<T>*>(entities.get());
    POMDOG_ASSERT(nativeEntities != nullptr);
    POMDOG_ASSERT(nativeEntities == dynamic_cast<Detail::ComponentBuffer<T>*>(entities.get()));
    POMDOG_ASSERT(desc.ComponentBitMask[typeIndex]);
    POMDOG_ASSERT(HasComponent<T>(entity));
    return nativeEntities->GetComponent(entity.GetIndex());
}

template <std::uint8_t MaxComponentCapacity>
template <typename T>
void EntityManager<MaxComponentCapacity>::SetComponentData(const Entity& entity, T&& data)
{
    const auto typeIndex = ComponentTypeDeclaration<T>::GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);

    if (typeIndex >= components.size()) {
        return;
    }

    auto& entities = components[typeIndex];
    if (entities == nullptr) {
        return;
    }

    POMDOG_ASSERT(entity.GetIndex() < descriptions.size());
    auto& desc = descriptions[entity.GetIndex()];
    if (desc.IncremantalVersion != entity.GetVersion()) {
        return;
    }

    POMDOG_ASSERT(entities != nullptr);
    if (entity.GetIndex() >= entities->GetSize()) {
        return;
    }

    POMDOG_ASSERT(entity.GetIndex() < entities->GetSize());

    auto nativeEntities = static_cast<Detail::ComponentBuffer<T>*>(entities.get());
    POMDOG_ASSERT(nativeEntities != nullptr);
    POMDOG_ASSERT(nativeEntities == dynamic_cast<Detail::ComponentBuffer<T>*>(entities.get()));
    POMDOG_ASSERT(desc.ComponentBitMask[typeIndex]);
    POMDOG_ASSERT(HasComponent<T>(entity));
    auto component = nativeEntities->GetComponent(entity.GetIndex());
    *component = std::move(data);
}

template <std::uint8_t MaxComponentCapacity>
template <typename T, typename... Components>
std::vector<Entity> EntityManager<MaxComponentCapacity>::QueryComponents()
{
    const auto mask = Helper::ComponentMask<MaxComponentCapacity, T, Components...>();

    std::vector<Entity> result;
    std::uint32_t index = 0;
    for (const auto& desc : descriptions) {
        if (desc.IsEnabled) {
            if ((desc.ComponentBitMask & mask) == mask) {
                Entity entity{desc.IncremantalVersion, index};
                result.push_back(entity);

                if (result.size() >= entityCount) {
                    break;
                }
            }
        }
        ++index;
    }
    return result;
}

} // namespace Pomdog::ECS::Detail

namespace Pomdog::ECS {

using EntityManager = Detail::EntityManager<64>;

} // namespace Pomdog::ECS
