// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/web_socket.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <deque>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// WebSocketEmscripten connects with the WebSocket of the browser.
///
/// The browser connects to the server, verifies it, and answers Ping frames.
/// JavaScript queues what happens to the socket, and `update()` takes it.
/// Timers in JavaScript apply `WebSocketOptions::connectTimeout`.
class WebSocketEmscripten final : public WebSocket {
public:
    WebSocketEmscripten() noexcept;

    ~WebSocketEmscripten() override;

    /// Starts connecting to url with options. Fails when the environment has
    /// no WebSocket.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(std::string_view url, const WebSocketOptions& options);

    void update() override;

    [[nodiscard]] std::optional<WebSocketEvent>
    pollEvent() override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    sendText(std::string_view text) override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    sendBinary(std::span<const u8> data) override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    close(u16 code, std::string_view reason) override;

    [[nodiscard]] bool
    isConnected() const noexcept override;

    [[nodiscard]] std::string_view
    getProtocol() const noexcept override;

private:
    enum class State : u8 {
        Connecting,
        Open,
        Closing,
        Closed,
    };

    std::deque<WebSocketEvent> events_;
    std::string protocol_;

    /// The ID of the JavaScript state of this socket.
    i32 socketID_ = 0;

    State state_ = State::Connecting;
};

} // namespace pomdog::network::detail
