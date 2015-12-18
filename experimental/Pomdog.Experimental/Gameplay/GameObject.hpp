// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/GameComponent.hpp"
#include "GameObjectID.hpp"
#include "GameObjectContext.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <memory>
#include <utility>

namespace Pomdog {

class GameObject {
public:
    GameObject() = default;

    explicit GameObject(std::shared_ptr<GameObjectContext> const& context);
    explicit GameObject(std::shared_ptr<GameObjectContext> && context);

    GameObject(std::shared_ptr<GameObjectContext> const& context, GameObjectID const& id);
    GameObject(std::shared_ptr<GameObjectContext> && context, GameObjectID const& id);

    explicit operator bool() const;

    bool operator==(GameObject const& gameObject) const;
    bool operator!=(GameObject const& gameObject) const;

    GameObjectID ID() const;

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
    std::shared_ptr<GameObjectContext> context;
    GameObjectID id;
};

template <class T>
class Component;

template <typename T>
T const* GameObject::Component() const
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->Component<T>(id);
}

template <typename T>
T* GameObject::Component()
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->Component<T>(id);
}

template <typename T>
bool GameObject::HasComponent() const
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    static_assert(std::is_base_of<Pomdog::Component<T>, T>::value, "TOOD: Not implemented");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->HasComponent<T>(id);
}

template <typename T>
T & GameObject::AddComponent(std::unique_ptr<T> && component)
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->AddComponent<T>(id, std::move(component));
}

template <typename T, typename...Arguments>
T & GameObject::AddComponent(Arguments && ...arguments)
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    return context->AddComponent<T>(id, std::forward<Arguments>(arguments)...);
}

template <typename T>
void GameObject::RemoveComponent()
{
    static_assert(std::is_base_of<Detail::Gameplay::GameComponent, T>::value, "");
    POMDOG_ASSERT(context);
    POMDOG_ASSERT(context->Valid(id));
    context->RemoveComponent<T>(id);
}

} // namespace Pomdog
