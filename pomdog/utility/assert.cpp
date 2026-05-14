// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <exception>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

#if defined(POMDOG_USE_CUSTOM_ASSERT)
[[noreturn]] void
reportAssertion(
    [[maybe_unused]] const char* message,
    [[maybe_unused]] std::source_location location) noexcept
{
    std::terminate();
}
#endif

} // namespace pomdog::detail
