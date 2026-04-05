// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

/// Returns true if x is a power of two (e.g., 1, 2, 4, 8...), false otherwise.
///
/// This function is often used to validate memory alignment requirements,
/// which must be powers of two on most platforms.
[[nodiscard]] constexpr bool
isPowerOfTwo(std::size_t x) noexcept
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

/// Rounds up a pointer to the nearest address that is aligned to the specified boundary.
///
/// @param ptr The pointer to align.
/// @param alignment The alignment boundary in bytes. Must be a power of two.
///        Passing 0 results in undefined behavior.
/// @return A pointer that is >= `ptr` and aligned to `alignment` bytes.
[[nodiscard]] void*
alignTo(void* ptr, std::size_t alignment) noexcept;

} // namespace pomdog::memory
