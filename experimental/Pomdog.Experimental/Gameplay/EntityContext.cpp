// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "EntityContext.hpp"

namespace Pomdog {
namespace Detail {
namespace Gameplay {

template <std::uint8_t MaxComponentCapacity>
EntityContext<MaxComponentCapacity>::EntityContext()
    : entityCount(0)
{
    static_assert(MaxComponentCapacity > 0, "");
    components.reserve(MaxComponentCapacity);
}

template <std::uint8_t MaxComponentCapacity>
EntityID EntityContext<MaxComponentCapacity>::Create(
    std::vector<std::shared_ptr<ComponentCreatorBase>> && componentCreators)
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
    EntityID entity{desc.IncremantalCounter, index};

    for (auto & creator : componentCreators) {
        auto component = creator->CreateComponent();
        auto componentType = creator->GetComponentType();
        POMDOG_ASSERT(component);
        AddComponent(entity, componentType, std::move(component));
    }

    return entity;
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::AddComponent(
    const EntityID& entityId,
    std::uint8_t componentTypeIndex,
    std::shared_ptr<Component> && component)
{
    const auto typeIndex = componentTypeIndex;
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
    auto & entities = components[typeIndex];

    if (entityId.Index() >= entities.size()) {
        static_assert(std::is_unsigned<decltype(entityId.Index())>::value, "" );
        entities.resize(entityId.Index() + 1U);
    }

    POMDOG_ASSERT(component != nullptr);
    POMDOG_ASSERT(entityId.Index() < entities.size());
    entities[entityId.Index()] = std::move(component);

    POMDOG_ASSERT(entityId.Index() < descriptions.size());
    auto & desc = descriptions[entityId.Index()];

    POMDOG_ASSERT(desc.IncremantalCounter > 0);
    POMDOG_ASSERT(typeIndex < desc.ComponentBitMask.size());
    POMDOG_ASSERT(typeIndex < MaxComponentCapacity);
    desc.ComponentBitMask[typeIndex] = 1;

    POMDOG_ASSERT(entities[entityId.Index()]);
    POMDOG_ASSERT(entities[entityId.Index()].get() != nullptr);
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityContext<MaxComponentCapacity>::GetCount() const noexcept
{
    return entityCount;
}

template <std::uint8_t MaxComponentCapacity>
std::size_t EntityContext<MaxComponentCapacity>::GetCapacity() const noexcept
{
    return descriptions.capacity();
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Clear()
{
    for (std::uint32_t index = 0; index < descriptions.size(); ++index) {
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
    for (auto & entities : components) {
        if (index < entities.size()) {
            entities[index].reset();
        }
    }
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Refresh()
{
    for (auto & id : destroyedObjects) {
        POMDOG_ASSERT(!Valid(id));
        auto index = id.Index();
        DestroyComponents(index);
        deletedIndices.push_back(index);
    }
    destroyedObjects.clear();
}

template <std::uint8_t MaxComponentCapacity>
void EntityContext<MaxComponentCapacity>::Destroy(const EntityID& id)
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
void EntityContext<MaxComponentCapacity>::DestroyImmediate(const EntityID& id)
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
bool EntityContext<MaxComponentCapacity>::Valid(const EntityID& id) const noexcept
{
    return (id.Index() < descriptions.size())
        && (descriptions[id.Index()].IncremantalCounter == id.SequenceNumber());
}

// explicit instantiations
template class EntityContext<64>;

} // namespace Gameplay
} // namespace Detail
} // namespace Pomdog
