// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {

/// IndexFormat specifies the format of index buffer indices.
enum class IndexFormat : std::uint8_t {
    /// Specifies that indices are 16-bit unsigned integer values.
    UInt16,

    /// Specifies that indices are 32-bit unsigned integer values.
    UInt32,
};

} // namespace pomdog::gpu
