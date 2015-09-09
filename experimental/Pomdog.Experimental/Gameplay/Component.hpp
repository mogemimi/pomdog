// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COMPONENT_0CB8A002_HPP
#define POMDOG_COMPONENT_0CB8A002_HPP

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

#endif // POMDOG_COMPONENT_0CB8A002_HPP
