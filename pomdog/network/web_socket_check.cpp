// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/web_socket_check.h"
#include "pomdog/utility/utfcpp_headers.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cstddef>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// The longest reason that fits in a Close frame, whose payload has at most
/// 125 bytes, after the 2-byte status code.
constexpr std::size_t maxCloseReasonSize = 123;

[[nodiscard]] char
toLower(char c) noexcept
{
    if ((c >= 'A') && (c <= 'Z')) {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

/// Returns true if text starts with prefix, ignoring case.
[[nodiscard]] bool
startsWithIgnoringCase(std::string_view text, std::string_view prefix) noexcept
{
    return (text.size() >= prefix.size()) &&
           std::ranges::equal(text.substr(0, prefix.size()), prefix, [](char x, char y) { return toLower(x) == toLower(y); });
}

[[nodiscard]] bool
isDigit(char c) noexcept
{
    return (c >= '0') && (c <= '9');
}

/// Returns true if c may appear in a token (RFC 9110, section 5.6.2).
[[nodiscard]] bool
isTokenCharacter(char c) noexcept
{
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || isDigit(c)) {
        return true;
    }
    return std::string_view{"!#$%&'*+-.^_`|~"}.find(c) != std::string_view::npos;
}

/// Returns true if port is a decimal port number from 0 to 65535.
[[nodiscard]] bool
isValidPort(std::string_view port) noexcept
{
    if (port.empty() || (port.size() > 5) || !std::ranges::all_of(port, isDigit)) {
        return false;
    }
    int value = 0;
    for (const char c : port) {
        value = (value * 10) + (c - '0');
    }
    return value <= 65535;
}

} // namespace

std::expected<WebSocketURL, std::unique_ptr<Error>>
parseWebSocketURL(std::string_view url)
{
    const auto isSpaceOrControl = [](char c) {
        const auto u = static_cast<unsigned char>(c);
        return (u <= 0x20) || (u == 0x7f);
    };
    if (std::ranges::any_of(url, isSpaceOrControl)) {
        return std::unexpected(errors::make("the WebSocket URL has a space or a control character"));
    }

    WebSocketURL result;
    std::string_view rest;
    if (startsWithIgnoringCase(url, "wss://")) {
        result.secure = true;
        rest = url.substr(6);
    }
    else if (startsWithIgnoringCase(url, "ws://")) {
        rest = url.substr(5);
    }
    else {
        return std::unexpected(errors::make("the WebSocket URL must start with ws:// or wss://"));
    }
    if (rest.find('#') != std::string_view::npos) {
        return std::unexpected(errors::make("the WebSocket URL must not have a fragment"));
    }

    const auto authorityEnd = rest.find_first_of("/?");
    const auto authority = rest.substr(0, authorityEnd);
    if (authority.find('@') != std::string_view::npos) {
        return std::unexpected(errors::make("the WebSocket URL must not have user information"));
    }

    // NOTE: An IPv6 address has colons, so it comes in brackets, such as
    // "[::1]:8080".
    std::string_view host;
    std::string_view port;
    if (authority.starts_with('[')) {
        const auto close = authority.find(']');
        if (close == std::string_view::npos) {
            return std::unexpected(errors::make("the WebSocket URL has an invalid host"));
        }
        host = authority.substr(0, close + 1);
        const auto afterHost = authority.substr(close + 1);
        if (!afterHost.empty()) {
            if (afterHost.front() != ':') {
                return std::unexpected(errors::make("the WebSocket URL has an invalid host"));
            }
            port = afterHost.substr(1);
        }
    }
    else {
        const auto colon = authority.find(':');
        host = authority.substr(0, colon);
        if (colon != std::string_view::npos) {
            port = authority.substr(colon + 1);
        }
    }
    if (host.empty() || (host == "[]")) {
        return std::unexpected(errors::make("the WebSocket URL has no host"));
    }
    if (!port.empty() && !isValidPort(port)) {
        return std::unexpected(errors::make("the WebSocket URL has an invalid port"));
    }

    const std::string_view defaultPort = result.secure ? "443" : "80";
    result.address = std::string{host} + ":" + std::string{port.empty() ? defaultPort : port};
    result.host = std::string{host};
    if (!port.empty()) {
        result.host += ':';
        result.host += port;
    }

    if (authorityEnd == std::string_view::npos) {
        result.resource = "/";
    }
    else {
        const auto resource = rest.substr(authorityEnd);
        result.resource = resource.starts_with('?') ? ("/" + std::string{resource}) : std::string{resource};
    }
    return result;
}

std::expected<void, std::unique_ptr<Error>>
checkWebSocketProtocols(std::span<const std::string> protocols)
{
    for (std::size_t i = 0; i < protocols.size(); ++i) {
        const auto& protocol = protocols[i];
        if (protocol.empty() || !std::ranges::all_of(protocol, isTokenCharacter)) {
            return std::unexpected(errors::make("the WebSocket subprotocol \"" + protocol + "\" is not a valid token"));
        }
        if (std::ranges::find(protocols.subspan(0, i), protocol) != protocols.subspan(0, i).end()) {
            return std::unexpected(errors::make("the WebSocket subprotocol " + protocol + " is offered twice"));
        }
    }
    return {};
}

std::expected<void, std::unique_ptr<Error>>
checkWebSocketClose(u16 code, std::string_view reason)
{
    if ((code != 1000) && !((code >= 3000) && (code <= 4999))) {
        return std::unexpected(errors::make("the close code " + std::to_string(code) + " is neither 1000 nor from 3000 to 4999"));
    }
    if (reason.size() > maxCloseReasonSize) {
        return std::unexpected(errors::make("the close reason is longer than " + std::to_string(maxCloseReasonSize) + " bytes"));
    }
    if (!isValidUTF8(reason)) {
        return std::unexpected(errors::make("the close reason is not valid UTF-8"));
    }
    return {};
}

bool isValidUTF8(std::string_view text) noexcept
{
    return utf8::is_valid(std::begin(text), std::end(text));
}

} // namespace pomdog::network::detail
