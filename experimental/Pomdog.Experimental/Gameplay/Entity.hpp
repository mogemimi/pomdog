// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog.Experimental/Gameplay/EntityID.hpp"
#include "Pomdog.Experimental/Gameplay/EntityContext.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>

namespace Pomdog {

class Entity final {
public:
    Entity() = default;

    Entity(EntityContext* context, const EntityID& id) noexcept;

    explicit operator bool() const noexcept;

    bool operator==(const Entity& entity) const noexcept;
    bool operator!=(const Entity& entity) const noexcept;

    EntityID GetID() const noexcept;

    template <typename T>
    std::shared_ptr<T> GetComponent() const;

    template <typename T>
    std::shared_ptr<T> GetComponent();

    template <typename T>
    bool HasComponent() const;

    void Destroy();

    void DestroyImmediate();

private:
    // NOTE: This pointer should be weak or raw pointer instead of shared_ptr.
    EntityContext* context = nullptr;
    EntityID id;
};

template <typename T>
std::shared_ptr<T> Entity::GetComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "");
    POMDOG_ASSERT(context != nullptr);
    POMDOG_ASSERT(context->Valid(id));
    return context->GetComponent<T>(id);
}

template <typename T>
std::shared_ptr<T> Entity::GetComponent()
{
    static_assert(std::is_base_of<Component, T>::value, "");
    POMDOG_ASSERT(context != nullptr);
    POMDOG_ASSERT(context->Valid(id));
    return context->GetComponent<T>(id);
}

template <typename T>
bool Entity::HasComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "");
    POMDOG_ASSERT(context != nullptr);
    POMDOG_ASSERT(context->Valid(id));
    return context->HasComponent<T>(id);
}

} // namespace Pomdog
