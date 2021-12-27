// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/ecs/component_buffer.h"
#include "pomdog/experimental/ecs/entity.h"
#include "pomdog/experimental/ecs/entity_desc.h"
#include "pomdog/experimental/ecs/entity_query.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <bitset>
#include <cstdint>
#include <deque>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs::detail {

template <std::uint8_t MaxComponentCapacity>
class EntityArchtype;

template <std::uint8_t MaxComponentCapacity>
class EntityManager final {
public:
    EntityManager();

    [[nodiscard]] Entity
    CreateEntity(const EntityArchtype<MaxComponentCapacity>& archtype);

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

    void ForEach(std::function<void(Entity)>&& func);

    template <typename T, typename... Components>
    EntityQuery<MaxComponentCapacity, T, Components...> WithAll() noexcept;

    std::size_t GetCount() const noexcept;

    std::size_t GetCapacity() const noexcept;

private:
    void AddComponent(const Entity& entity, ComponentTypeBase& componentType);

private:
    std::vector<std::unique_ptr<ComponentBufferBase>> components;
    std::vector<EntityDesc<MaxComponentCapacity>> descriptions;
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
    return Helper::GetComponent<MaxComponentCapacity, T>(entity, components, descriptions);
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

    auto nativeEntities = static_cast<detail::ComponentBuffer<T>*>(entities.get());
    POMDOG_ASSERT(nativeEntities != nullptr);
    POMDOG_ASSERT(nativeEntities == dynamic_cast<detail::ComponentBuffer<T>*>(entities.get()));
    POMDOG_ASSERT(desc.ComponentBitMask[typeIndex]);
    POMDOG_ASSERT(HasComponent<T>(entity));
    auto component = nativeEntities->GetComponent(entity.GetIndex());
    *component = std::move(data);
}

template <std::uint8_t MaxComponentCapacity>
void EntityManager<MaxComponentCapacity>::ForEach(std::function<void(Entity)>&& func)
{
    std::uint32_t index = 0;
    for (const auto& desc : descriptions) {
        if (desc.IsEnabled) {
            Entity entity{desc.IncremantalVersion, index};
            func(std::move(entity));
        }
        ++index;
    }
}

template <std::uint8_t MaxComponentCapacity>
template <typename T, typename... Components>
EntityQuery<MaxComponentCapacity, T, Components...> EntityManager<MaxComponentCapacity>::WithAll() noexcept
{
    EntityQuery<MaxComponentCapacity, T, Components...> query{components, descriptions};
    return query;
}

} // namespace pomdog::ecs::detail

namespace pomdog::ecs {

using EntityManager = detail::EntityManager<64>;

} // namespace pomdog::ecs
