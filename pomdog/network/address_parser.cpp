// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/address_parser.hpp"

namespace pomdog::detail::AddressParser {
namespace {

std::tuple<std::string_view, std::string_view>
SeparateString(std::string_view source, std::string_view separator)
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

std::tuple<AddressFamily, std::string_view, std::string_view>
TransformAddress(std::string_view address)
{
    if (!address.empty() && (address.front() == '[')) {
        // NOTE: IPv6 (e.g. [host]:port)
        auto [host, port] = detail::AddressParser::SeparateString(address.substr(1), "]:");
        return std::make_tuple(AddressFamily::InterNetworkV6, std::move(host), std::move(port));
    }

    auto [host, port] = detail::AddressParser::SeparateString(address, ":");
    return std::make_tuple(AddressFamily::InterNetworkV4, std::move(host), std::move(port));
}

} // namespace pomdog::detail::AddressParser
