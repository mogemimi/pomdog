// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

/// Represents an array of bytes with a specified alignment and size.
/// It is used to create uninitialized storage for objects that require specific alignment.
template <std::size_t Alignment, std::size_t Size>
struct AlignedByteArray final {
    static_assert(sizeof(std::byte) == 1);
    alignas(Alignment) std::byte buffer[Size];
};

} // namespace pomdog::detail
