// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// WebSocketURL holds the parts of a ws or wss URL that a client needs.
struct WebSocketURL final {
    /// The host and the port to connect to, such as "example.com:443" or
    /// "[::1]:80".
    std::string address;

    /// The value of the Host header, the host and the port as the URL has
    /// them, such as "example.com" or "[::1]:8080".
    std::string host;

    /// The path and the query to request, such as "/chat?room=1", which is
    /// "/" when the URL has no path.
    std::string resource;

    /// True for a wss URL, false for a ws URL.
    bool secure = false;
};

/// Parses url, a ws or wss URL.
///
/// Fails when url has another scheme, a fragment, user information, no host,
/// an invalid port, or a space or a control character. Browsers fail the same
/// URLs, except that some also take http and https URLs.
[[nodiscard]] std::expected<WebSocketURL, std::unique_ptr<Error>>
parseWebSocketURL(std::string_view url);

/// Checks the subprotocols that a client offers. Fails when one is not a
/// token (RFC 9110, section 5.6.2) or two are the same, as browsers do.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkWebSocketProtocols(std::span<const std::string> protocols);

/// Checks the arguments of `WebSocket::close()`. Fails, as browsers do, when
/// code is neither 1000 nor from 3000 to 4999, and when reason is not valid
/// UTF-8 or longer than 123 bytes.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
checkWebSocketClose(u16 code, std::string_view reason);

/// Returns true if text is valid UTF-8, without overlong forms, surrogates,
/// or code points above U+10FFFF, false otherwise.
[[nodiscard]] bool
isValidUTF8(std::string_view text) noexcept;

} // namespace pomdog::network::detail
