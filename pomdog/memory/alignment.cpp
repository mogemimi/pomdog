// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/alignment.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::memory {

[[nodiscard]] void*
alignTo(void* ptr, std::size_t alignment) noexcept
{
    const auto p = static_cast<char*>(ptr);
    return reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(p + (alignment - 1)) & ~(alignment - 1));
}

} // namespace pomdog::memory
