// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/address_parser.h"

namespace pomdog::detail::AddressParser {
namespace {

[[nodiscard]] std::tuple<std::string_view, std::string_view>
separateString(std::string_view source, std::string_view separator)
{
    std::string_view first;
    std::string_view second;

    std::string_view::size_type start = 0;
    if (auto end = source.find(separator, start); end != std::string_view::npos) {
        first = source.substr(start, end - start);
        second = source.substr(end + separator.size());
    }
    else {
        first = source;
    }
    return std::make_tuple(std::move(first), std::move(second));
}

} // namespace

[[nodiscard]] std::tuple<AddressFamily, std::string_view, std::string_view>
transformAddress(std::string_view address)
{
    if (!address.empty() && (address.front() == '[')) {
        // NOTE: IPv6 (e.g. [host]:port)
        auto [host, port] = separateString(address.substr(1), "]:");
        return std::make_tuple(AddressFamily::InterNetworkV6, std::move(host), std::move(port));
    }

    auto [host, port] = separateString(address, ":");
    return std::make_tuple(AddressFamily::InterNetworkV4, std::move(host), std::move(port));
}

} // namespace pomdog::detail::AddressParser
