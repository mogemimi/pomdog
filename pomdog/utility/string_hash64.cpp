// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/string_hash64.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

[[nodiscard]] u64
computeStringHash64(std::string_view s) noexcept
{
    return pomdog::hash::xxh64(s.data(), s.size(), detail::string_hash64_seed);
}

} // namespace pomdog
