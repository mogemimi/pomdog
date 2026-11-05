// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/host_port.h"

namespace pomdog::network::detail {
namespace {

/// Splits source at the first separator, or returns all of source as the host
/// when it has no separator.
[[nodiscard]] HostPort
splitAt(std::string_view source, std::string_view separator) noexcept
{
    const auto end = source.find(separator);
    if (end == std::string_view::npos) {
        return HostPort{.host = source, .port = {}};
    }
    return HostPort{.host = source.substr(0, end), .port = source.substr(end + separator.size())};
}

} // namespace

HostPort
splitHostPort(std::string_view address) noexcept
{
    if (address.starts_with('[')) {
        // NOTE: An IPv6 address has colons, so it comes in brackets, such as
        // "[::1]:8080".
        return splitAt(address.substr(1), "]:");
    }
    return splitAt(address, ":");
}

} // namespace pomdog::network::detail
