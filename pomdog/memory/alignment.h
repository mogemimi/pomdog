// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

[[nodiscard]] constexpr bool
isPowerOfTwo(std::size_t x) noexcept
{
    return (x != 0) && ((x & (x - 1)) == 0);
}

[[nodiscard]] void*
alignTo(void* ptr, std::size_t alignment) noexcept;

} // namespace pomdog::memory
