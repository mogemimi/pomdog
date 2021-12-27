// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// ContainmentType indicates whether a geometry object contains another one.
enum class ContainmentType : std::uint8_t {
    Contains,

    Disjoint,

    Intersects
};

} // namespace pomdog
