// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace Pomdog {

class EntityManager final {
public:
    EntityManager() = default;

    EntityManager(EntityManager const&) = delete;
    EntityManager(EntityManager &&) = default;

    EntityManager & operator=(EntityManager const&) = delete;
    EntityManager & operator=(EntityManager &&) = default;

    Entity CreateEntity(
        std::vector<std::shared_ptr<ComponentCreatorBase>> && componentCreators);

    template <typename T, typename...Components>
    std::vector<Entity> QueryComponents();

    bool Valid(const EntityID& id) const noexcept;

    void Refresh();

    void Clear();

    std::size_t GetCount() const noexcept;

    std::size_t GetCapacity() const noexcept;

private:
    EntityContext context;
    std::vector<EntityID> entities;
};

template <typename T, typename...Components>
std::vector<Entity> EntityManager::QueryComponents()
{
    static_assert(std::is_object<T>::value, "");
    std::vector<Entity> result;
    for (auto & id : entities) {
        if (context.Valid(id)) {
            if (context.HasComponents<T, Components...>(id)) {
                result.emplace_back(&context, id);
            }
        }
    }
    return result;
}

} // namespace Pomdog
