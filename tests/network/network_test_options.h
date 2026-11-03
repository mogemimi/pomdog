// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/testing/testing.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: tools/cmd/network-test-runner starts the local servers and passes their
// addresses on the command line. Its README describes how each server answers.

namespace pomdog::testing {

/// Returns the address of the local HTTP server, such as `127.0.0.1:50000`,
/// that `--http-server` named.
[[nodiscard]] std::string_view
getHTTPServerAddress() noexcept;

/// Returns the address of the local TCP server that echoes each line, which
/// `--tcp-server` named.
[[nodiscard]] std::string_view
getTCPServerAddress() noexcept;

/// Returns the address of the local UDP server that echoes each datagram,
/// which `--udp-server` named.
[[nodiscard]] std::string_view
getUDPServerAddress() noexcept;

} // namespace pomdog::testing
