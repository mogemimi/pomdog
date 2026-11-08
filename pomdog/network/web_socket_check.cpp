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

/// Returns true if c may appear in a token (RFC 9110, section 5.6.2).
[[nodiscard]] bool
isTokenCharacter(char c) noexcept
{
    if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9'))) {
        return true;
    }
    return std::string_view{"!#$%&'*+-.^_`|~"}.find(c) != std::string_view::npos;
}

} // namespace

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
