// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/ECS/EntityManager.hpp"
#include "Pomdog/Experimental/ECS/ComponentType.hpp"
#include "Pomdog/Experimental/ECS/EntityArchtype.hpp"

namespace Pomdog::ECS::Detail {
namespace {

void ResetComponents(
    std::vector<std::unique_ptr<ComponentBufferBase>>& components,
    std::uint32_t index)
{
    for (auto& entities : components) {
        if ((entities != nullptr) && (index < entities->GetSize())) {
            entities->Reset(index);
        }
    }
}

} // namespace

template <std::uint8_t MaxComponentCapacity>
EntityManager<MaxComponentCapacity>::EntityManager()
    : entityCount(0)
{
    static_assert(MaxComponentCapacity > 0, "");
    components.reserve(MaxComponentCapacity);
}

template <std::uint8_t MaxComponentCapacity>
Entity EntityManager<MaxComponentCapacity>::CreateEntity(
    const EntityArchtype<MaxComponentCapacity>& archtype)
{
    std::uint32_t index = 0;
    if (deletedIndices.empty()) {
        POMDOG_ASSERT(std::numeric_limits<decltype(index)>::max() > descriptions.size());
        index = static_cast<std::uint32_t>(descriptions.size());
        descriptions.resize(descriptions.size() + 1);
    }
    else {
        index = deletedIndices.front();
        deletedIndices.pop_front();
    }

    auto& desc = descriptions[index];
    POMDOG_ASSERT(desc.ComponentBitMask.none());
    POMDOG_ASSERT(desc.IncremantalVersion > 0);

    ++entityCount;
    Entity entity{desc.IncremantalVersion, index};
    desc.IsEnabled = true;

    for (auto& componentType : archtype.GetComponentTypes()) {
        POMDOG_ASSERT(componentType != nullptr);
        AddComponent(entity, *componentType);
    }

    return entity;
}

template <std::uint8_t MaxComponentCapacity>
void EntityManager<MaxComponentCapacity>::AddComponent(
    const Entity& entity, ComponentTypeBase& componentType)
{
    const auto typeIndex = componentType.GetTypeIndex();
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
    if (typeIndex >= components.size()) {
        components.resize(typeIndex + 1U);

        POMDOG_ASSERT(components.size() <= MaxComponentCapacity);
        if (components.capacity() > MaxComponentCapacity) {
            components.shrink_to_fit();
            POMDOG_ASSERT(components.capacity() == MaxComponentCapacity);
        }
    }

    POMDOG_ASSERT(typeIndex < components.size());
    auto& entities = components[typeIndex];
    if (entities == nullptr) {
        entities = componentType.CreateComponentBuffer();
    }

    if (entity.GetIndex() >= entities->GetSize()) {
        static_assert(std::is_unsigned<decltype(entity.GetIndex())>::value, "");
        entities->Resize(entity.GetIndex() + 1U);
    }

    POMDOG_ASSERT(entity.GetIndex() < entities->GetSize());
    POMDOG_ASSERT(entity.GetIndex() < descriptions.size());
    auto& desc = descriptions[entity.GetIndex()];

    POMDOG_ASSERT(desc.IncremantalVersion > 0);
    POMDOG_ASSERT(typeIndex < desc.ComponentBitMask.size());
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
    desc.ComponentBitMask[typeIndex] = true;
}

template <std::uint8_t MaxComponentCapacity>
void EntityManager<MaxComponentCapacity>::DestroyEntity(const Entity& entity)
{
    POMDOG_ASSERT(Exists(entity));

    const auto index = entity.GetIndex();

    POMDOG_ASSERT(index < descriptions.size());
    POMDOG_ASSERT(descriptions[index].IncremantalVersion == entity.GetVersion());

    auto& desc = descriptions[index];
    desc.IsEnabled = false;
    desc.ComponentBitMask.reset();
    ++desc.IncremantalVersion;

    ResetComponents(components, index);
    deletedIndices.push_back(index);

    POMDOG_ASSERT(entityCount > 0);
    --entityCount;
}

template <std::uint8_t MaxComponentCapacity>
void EntityManager<MaxComponentCapacity>::DestroyAllEntities()
{
    for (std::uint32_t index = 0; index < descriptions.size(); ++index) {
        auto& desc = descriptions[index];

        if (desc.ComponentBitMask.any()) {
            ResetComponents(components, index);
        }
        deletedIndices.push_back(index);

        desc.ComponentBitMask.reset();
        ++desc.IncremantalVersion;
    }
    entityCount = 0;
}

template <std::uint8_t MaxComponentCapacity>
bool EntityManager<MaxComponentCapacity>::Exists(const Entity& entity) const noexcept
{
    return (entity.GetIndex() < descriptions.size()) &&
        (descriptions[entity.GetIndex()].IncremantalVersion == entity.GetVersion());
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityManager<MaxComponentCapacity>::GetCount() const noexcept
{
    return entityCount;
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityManager<MaxComponentCapacity>::GetCapacity() const noexcept
{
    return descriptions.capacity();
}

// explicit instantiations
template class EntityManager<64>;

} // namespace Pomdog::ECS::Detail
