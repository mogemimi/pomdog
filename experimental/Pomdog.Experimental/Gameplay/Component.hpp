// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ComponentTypeIndex.hpp"
#include "detail/GameComponent.hpp"
#include <cstdint>
#include <type_traits>
#include <utility>

namespace Pomdog {

template <class T>
class Component: public Detail::Gameplay::GameComponent {
public:
    static_assert(std::is_class<T>::value, "T is class");

    virtual ~Component() = default;

    static std::uint8_t TypeIndex()
    {
        return Detail::Gameplay::ComponentTypeIndex<GameComponent, std::uint8_t>::Index<T>();
    }
};

} // namespace Pomdog
