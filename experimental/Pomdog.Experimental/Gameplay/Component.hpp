// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

namespace Pomdog {

class Component {
public:
    virtual ~Component() = default;
};

} // namespace Pomdog

#include <cstdint>
#include <string>
#include <memory>

namespace Pomdog {

class ComponentCreatorBase {
public:
    virtual ~ComponentCreatorBase() = default;

    virtual std::uint8_t GetComponentType() = 0;

    virtual std::shared_ptr<Component> CreateComponent() = 0;
};

template <class TComponent>
struct ComponentTypeDeclaration;

template <class TComponent>
class ComponentCreator;

template <class TComponent, typename ...Args>
inline std::shared_ptr<ComponentCreatorBase> AddComponent(Args &&...args)
{
    return std::make_shared<ComponentCreator<TComponent>>(std::forward<Args>(args)...);
}

} // namespace Pomdog
