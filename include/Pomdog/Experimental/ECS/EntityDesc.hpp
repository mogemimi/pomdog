// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <bitset>
#include <cstdint>
#include <type_traits>

namespace Pomdog::ECS::Detail {

template <std::uint8_t MaxComponentCapacity>
class EntityDesc final {
public:
    std::bitset<MaxComponentCapacity> ComponentBitMask;
    std::uint32_t IncremantalVersion = 1;
    bool IsEnabled = false;
};

} // namespace Pomdog::ECS::Detail
