// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/connect_timeout.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <chrono>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

Duration
splitConnectTimeout(Duration timeLeft, std::size_t addressCount) noexcept
{
    if (addressCount <= 1) {
        return timeLeft;
    }
    constexpr auto minimumShare = Duration{std::chrono::seconds{2}};
    const auto share = timeLeft / static_cast<f64>(addressCount);
    return (share >= minimumShare) ? share : std::min(timeLeft, minimumShare);
}

} // namespace pomdog::network::detail
