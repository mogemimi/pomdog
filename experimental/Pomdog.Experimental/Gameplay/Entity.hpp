// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/GameComponent.hpp"
#include "EntityID.hpp"
#include "EntityContext.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>

namespace Pomdog {

class Entity {
public:
    Entity() = default;

    explicit Entity(std::shared_ptr<EntityContext> const& context);
    explicit Entity(std::shared_ptr<EntityContext> && context);

    Entity(std::shared_ptr<EntityContext> const& context, EntityID const& id);
    Entity(std::shared_ptr<EntityContext> && context, EntityID const& id);

    explicit operator bool() const;

    bool operator==(Entity const& entity) const;
    bool operator!=(Entity const& entity) const;

    EntityID ID() const;

    template <typename T>
    T const* Component() const;

    template <typename T>
    T* Component();

    template <typename T>
    bool HasComponent() const;

    template <typename T>
    T & AddComponent(std::unique_ptr<T> && component);

    template <typename T, typename...Arguments>
    T & AddComponent(Arguments && ...arguments);

    template <typename T>
    void RemoveComponent();

    void Destroy();

    void DestroyImmediate();

private:
    std::shared_ptr<EntityContext> context;
    EntityID id;
};

template <class T>
class Component;

template <typename T>
T const* Entity::Component() const
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->Component<T>(id);
}

template <typename T>
T* Entity::Component()
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->Component<T>(id);
}

template <typename T>
bool Entity::HasComponent() const
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->HasComponent<T>(id);
}

template <typename T>
T & Entity::AddComponent(std::unique_ptr<T> && component)
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->AddComponent<T>(id, std::move(component));
}

template <typename T, typename...Arguments>
T & Entity::AddComponent(Arguments && ...arguments)
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->AddComponent<T>(id, std::forward<Arguments>(arguments)...);
}

template <typename T>
void Entity::RemoveComponent()
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    context->RemoveComponent<T>(id);
}

} // namespace Pomdog
