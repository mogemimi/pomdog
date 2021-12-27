// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <bitset>
#include <cstdint>
#include <type_traits>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::ecs::detail {

template <std::uint8_t MaxComponentCapacity>
class EntityDesc final {
public:
    std::bitset<MaxComponentCapacity> ComponentBitMask;
    std::uint32_t IncremantalVersion = 1;
    bool IsEnabled = false;
};

} // namespace pomdog::ecs::detail
