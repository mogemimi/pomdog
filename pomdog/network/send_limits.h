// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Limits the bytes that may wait to be sent when a stream takes a write, so
/// that a peer that stops reading cannot use up the memory. A write beyond it
/// closes the stream, as browsers close a WebSocket whose send buffer is full.
constexpr std::size_t maxPendingSendSize = 64 * 1024 * 1024;

} // namespace pomdog::network::detail
