// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Returns how long connecting to the next address may take, when timeLeft
/// remains of the connect timeout and addressCount addresses, including the
/// next one, are left to try.
///
/// Each address gets an even share of the time left, so that an address that
/// never answers cannot use up the time of the others. A share is at least two
/// seconds while that much time remains, as in Go's net package, so that a
/// slow network can still connect to one of many addresses.
[[nodiscard]] Duration
splitConnectTimeout(Duration timeLeft, std::size_t addressCount) noexcept;

} // namespace pomdog::network::detail
