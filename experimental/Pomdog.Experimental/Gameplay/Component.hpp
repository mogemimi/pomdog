// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "detail/ComponentTypeIndex.hpp"
#include "detail/ComponentBase.hpp"
#include <cstdint>
#include <type_traits>
#include <utility>

namespace Pomdog {

template <class T>
class Component : public Detail::Gameplay::ComponentBase {
public:
    static_assert(std::is_class<T>::value, "T is class");

    virtual ~Component() = default;

    static std::uint8_t TypeIndex()
    {
        using Detail::Gameplay::ComponentTypeIndex;
        return ComponentTypeIndex<ComponentBase, std::uint8_t>::Index<T>();
    }
};

} // namespace Pomdog
