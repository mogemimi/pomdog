// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/web_socket.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/http_request_check.h"
#include "pomdog/network/web_socket_check.h"

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include "pomdog/network/emscripten/web_socket_emscripten.h"
#else
#include "pomdog/network/native/web_socket_native.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

WebSocket::WebSocket() noexcept = default;

WebSocket::~WebSocket() = default;

std::expected<std::shared_ptr<WebSocket>, std::unique_ptr<Error>>
connectWebSocket(std::string_view url, const WebSocketOptions& options)
{
    auto parsedURL = detail::parseRequestURL(url, "ws", "wss");
    if (!parsedURL) {
        return std::unexpected(errors::wrap(std::move(parsedURL.error()), "invalid WebSocket URL"));
    }
    if (auto result = detail::checkWebSocketProtocols(options.protocols); !result) {
        return std::unexpected(std::move(result.error()));
    }
    if (parsedURL->secure && (options.trustStore == nullptr)) {
        return std::unexpected(errors::make("wss URLs need WebSocketOptions::trustStore"));
    }

#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    auto socket = std::make_shared<detail::WebSocketEmscripten>();
    if (auto result = socket->connect(url, options); !result) {
        return std::unexpected(std::move(result.error()));
    }
#else
    auto socket = std::make_shared<detail::WebSocketNative>();
    if (auto result = socket->connect(*parsedURL, options); !result) {
        return std::unexpected(std::move(result.error()));
    }
#endif
    return socket;
}

} // namespace pomdog::network
