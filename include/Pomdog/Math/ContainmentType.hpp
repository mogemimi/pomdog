// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class ContainmentType : std::uint8_t {
    Contains,

    Disjoint,

    Intersects
};

} // namespace Pomdog
