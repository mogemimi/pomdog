// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

[[nodiscard]] u32
computeStringHash32(std::string_view s) noexcept
{
    return pomdog::hash::xxh32(s.data(), s.size(), detail::string_hash32_seed);
}

} // namespace pomdog
