// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Limits the bytes that a stream receives in one `update()`, so that a fast
/// sender cannot hold up a frame. The rest waits in the socket for the next
/// `update()`.
constexpr std::size_t maxReceiveSizePerUpdate = 1024 * 1024;

/// Sets the size of the buffer that a stream reads into. It holds the largest
/// UDP datagram, so that no datagram is cut short.
constexpr std::size_t receiveBufferSize = 64 * 1024;

} // namespace pomdog::network::detail
