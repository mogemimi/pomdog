// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

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

} // namespace pomdog::gpu
