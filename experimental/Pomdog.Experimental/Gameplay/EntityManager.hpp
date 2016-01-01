// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Entity.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {

class Entity;

class EntityManager {
public:
    EntityManager();

    EntityManager(EntityManager const&) = delete;
    EntityManager(EntityManager &&) = default;

    EntityManager & operator=(EntityManager const&) = delete;
    EntityManager & operator=(EntityManager &&) = default;

    Entity CreateObject();

    //template <typename T, typename...Components>
    //std::vector<T> QueryComponent();

    template <typename T, typename...Components>
    std::vector<Entity> QueryComponents();

    template <typename T>
    T const* Component(EntityID const& id) const;

    template <typename T>
    T* Component(EntityID const& id);

    template <typename T>
    bool HasComponent(EntityID const& id) const;

    bool Valid(EntityID const& id) const;

    void Refresh();

    void Clear();

    std::size_t Count() const;

    std::size_t Capacity() const;

private:
    std::shared_ptr<EntityContext> context;
    std::vector<EntityID> objects;
};

template <typename T, typename...Components>
std::vector<Entity> EntityManager::QueryComponents()
{
    static_assert(std::is_object<T>::value, "");

    std::vector<Entity> result;

    for (auto & id: objects)
    {
        if (context->Valid(id)) {
            if (context->HasComponents<T, Components...>(id)) {
                result.emplace_back(context, id);
            }
        }
    }
    return std::move(result);
}

template <typename T>
T const* EntityManager::Component(EntityID const& id) const
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->Component<T>(id);
}

template <typename T>
T* EntityManager::Component(EntityID const& id)
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->Component<T>(id);
}

template <typename T>
bool EntityManager::HasComponent(EntityID const& id) const
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->HasComponent<T>(id);
}

} // namespace Pomdog
