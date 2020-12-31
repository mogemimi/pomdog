// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

/// ContainmentType indicates whether a geometry object contains another one.
enum class ContainmentType : std::uint8_t {
    Contains,

    Disjoint,

    Intersects
};

} // namespace Pomdog
