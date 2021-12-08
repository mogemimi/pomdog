// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

enum class InputElementFormat : std::uint8_t {
    /// A single-component, 32-bit floating-point format.
    Float,

    /// A two-component, 64-bit floating-point format.
    Float2,

    /// A three-componet, 96-bit floating-point format.
    Float3,

    /// A four-component, 128-bit floating-point format
    Float4,

    /// A four-component, 128-bit integer format
    Int4,

    /// A four-component, 128-bit unsigned integer format
    //UInt4,

    /// A four-component, 32-bit unsigned integer format.
    Byte4,

    /// A two-component, 32-bit floating-point format.
    HalfFloat2,

    /// A four-component, 64-bit floating-point format.
    HalfFloat4,
};

} // namespace pomdog
