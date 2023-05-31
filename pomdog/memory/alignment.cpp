// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/memory/alignment.h"

namespace pomdog::memory {

[[nodiscard]] void*
alignTo(void* ptr, std::size_t alignment) noexcept
{
    const auto p = static_cast<char*>(ptr);
    return reinterpret_cast<void*>(reinterpret_cast<std::size_t>(p + (alignment - 1)) & ~(alignment - 1));
}

} // namespace pomdog::memory
