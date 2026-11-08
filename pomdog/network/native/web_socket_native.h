// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/web_socket.h"
#include "pomdog/network/web_socket_frame.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <cstddef>
#include <deque>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
struct RequestURL;
} // namespace pomdog::network::detail

namespace pomdog::network::detail {

/// WebSocketNative speaks the WebSocket protocol over a TCP or TLS stream.
///
/// Once the stream connects, it sends the handshake request and reads the
/// response, and then reads frames with `WebSocketReader`. It masks the frames
/// that it sends with keys from the generator of PSA crypto, answers each Ping
/// with a Pong, and sends no Ping itself.
class WebSocketNative final : public WebSocket {
public:
    WebSocketNative() noexcept;

    ~WebSocketNative() override;

    /// Starts connecting to url with options.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(const RequestURL& url, const WebSocketOptions& options);

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
        /// The stream connects.
        Connecting,

        /// The handshake request went out, and the response is awaited.
        Handshaking,

        /// Messages go in both directions.
        Open,

        /// A Close frame went out or came in, and the server's close is
        /// awaited.
        Closing,

        /// The socket reported Disconnected.
        Closed,
    };

    void handleStreamEvent(StreamEvent&& event);

    void readHandshakeResponse(std::span<const u8> data);

    /// Reads the frames that the reader has, and handles each one.
    void readFrames();

    /// Sends a frame with opcode and payload, masked with a new key.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    sendFrame(WebSocketOpcode opcode, std::span<const u8> payload);

    /// Closes the connection without the closing handshake, and reports
    /// Disconnected with err. Sends a Close frame with closeCode first when it
    /// has one and the connection is open.
    void fail(std::optional<u16> closeCode, std::unique_ptr<Error>&& err);

    /// Closes the connection after the closing handshake, and reports
    /// Disconnected with the code and the reason of the server's Close frame.
    void finish();

    [[nodiscard]] std::chrono::steady_clock::time_point
    makeDeadline() const noexcept;

    std::deque<WebSocketEvent> events_;
    std::shared_ptr<Stream> stream_;
    std::optional<WebSocketReader> reader_;

    /// The bytes of the handshake response so far.
    std::vector<u8> handshakeBuffer_;
    std::string handshakeRequest_;

    /// The Sec-WebSocket-Accept value that the response must have.
    std::string expectedAccept_;

    std::vector<std::string> protocols_;
    std::string protocol_;
    std::string closeReason_;

    /// When connecting or closing gives up.
    std::chrono::steady_clock::time_point deadline_;

    Duration connectTimeout_ = Duration::zero();
    std::size_t maxMessageSize_ = 0;
    u16 closeCode_ = 0;
    State state_ = State::Connecting;
    bool isCloseSent_ = false;
    bool isCloseReceived_ = false;
};

} // namespace pomdog::network::detail
