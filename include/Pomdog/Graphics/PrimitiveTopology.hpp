// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

enum class PrimitiveTopology : std::uint8_t {
    /// A triangle list.
    TriangleList,

    /// A triangle strip.
    TriangleStrip,

    /// A line list.
    LineList,

    /// A line strip.
    LineStrip,
};

} // namespace Pomdog
