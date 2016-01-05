// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Entity.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {

class Entity;

class EntityManager final {
public:
    EntityManager();

    EntityManager(EntityManager const&) = delete;
    EntityManager(EntityManager &&) = default;

    EntityManager & operator=(EntityManager const&) = delete;
    EntityManager & operator=(EntityManager &&) = default;

    Entity CreateEntity();

    template <typename T, typename...Components>
    std::vector<Entity> QueryComponents();

    template <typename T>
    std::shared_ptr<T const> GetComponent(EntityID const& id) const;

    template <typename T>
    std::shared_ptr<T> GetComponent(EntityID const& id);

    template <typename T>
    bool HasComponent(EntityID const& id) const;

    bool Valid(EntityID const& id) const;

    void Refresh();

    void Clear();

    std::size_t GetCount() const noexcept;

    std::size_t GetCapacity() const noexcept;

private:
    std::shared_ptr<EntityContext> context;
    std::vector<EntityID> entities;
};

template <typename T, typename...Components>
std::vector<Entity> EntityManager::QueryComponents()
{
    static_assert(std::is_object<T>::value, "");
    std::vector<Entity> result;
    for (auto & id : entities) {
        if (context->Valid(id)) {
            if (context->HasComponents<T, Components...>(id)) {
                result.emplace_back(context, id);
            }
        }
    }
    return std::move(result);
}

template <typename T>
std::shared_ptr<T const> EntityManager::GetComponent(EntityID const& id) const
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->GetComponent<T>(id);
}

template <typename T>
std::shared_ptr<T> EntityManager::GetComponent(EntityID const& id)
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->GetComponent<T>(id);
}

template <typename T>
bool EntityManager::HasComponent(EntityID const& id) const
{
    static_assert(std::is_object<T>::value, "");
    POMDOG_ASSERT(context);
    return context->HasComponent<T>(id);
}

} // namespace Pomdog
