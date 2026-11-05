// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// HostPort holds the host and the port of an address. Both point into the
/// address that `splitHostPort()` split.
struct HostPort final {
    std::string_view host;
    std::string_view port;
};

/// Splits address into its host and port, such as "example.com:80" into
/// "example.com" and "80", or "[::1]:8080" into "::1" and "8080".
///
/// The port is empty when address has no separator. Nothing else is checked:
/// connecting to an invalid host or port fails later, in name resolution.
[[nodiscard]] HostPort
splitHostPort(std::string_view address) noexcept;

} // namespace pomdog::network::detail
