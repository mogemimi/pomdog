// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {
class TrustStore;
} // namespace pomdog::network

namespace pomdog::network {

/// Identifies whether a WebSocket message holds text or binary data.
enum class WebSocketMessageType : u8 {
    /// The message holds UTF-8 text.
    Text,

    /// The message holds bytes.
    Binary,
};

// NOTE: An event owns its error, so it is move-only. MSVC reports the
// implicitly deleted copy operations unless these warnings are off.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
/// Reports a change of a `WebSocket`.
struct WebSocketEvent final {
    /// Connected when the handshake succeeded, Received for each message, and
    /// Disconnected when the connection closed.
    StreamEventKind kind = StreamEventKind::Connected;

    /// The type of the message for Received. Binary for other kinds.
    WebSocketMessageType messageType = WebSocketMessageType::Binary;

    /// The message for Received, which is valid UTF-8 for a text message.
    /// Empty for other kinds.
    std::vector<u8> data;

    /// The status code of the server's Close frame for Disconnected, such as
    /// 1000 for a normal closure, or 1005 when the frame holds none. 1006 when
    /// the connection closed without the closing handshake, with an error.
    /// Zero for other kinds.
    u16 closeCode = 0;

    /// The reason of the server's Close frame for Disconnected, which may be
    /// empty. Empty for other kinds.
    std::string closeReason;

    /// Why the connection closed without the closing handshake for
    /// Disconnected, such as a failed connection, a timeout, or a frame that
    /// breaks the protocol, or nullptr after the closing handshake. nullptr
    /// for other kinds.
    std::unique_ptr<Error> error;
};
POMDOG_MSVC_SUPPRESS_WARNING_POP

/// Sets how a `WebSocket` connects and what it accepts.
struct WebSocketOptions final {
    /// Limits how long opening the connection may take, from name resolution
    /// to the server's handshake response, and how long `WebSocket::close()`
    /// waits for the server to close. Zero fails every connection at once.
    Duration connectTimeout = std::chrono::seconds{5};

    /// The subprotocols that the client offers, in order of preference, such
    /// as "chat". The connection fails, as in browsers, unless the server
    /// picks one of them, and `WebSocket::getProtocol()` tells which. Each
    /// must be a token of characters from "!" to "~" without separators, and
    /// they must differ.
    std::vector<std::string> protocols;

    /// Fails the connection when a message is larger than this many bytes, so
    /// that a server cannot use up the memory. Zero sets no limit.
    std::size_t maxMessageSize = 64 * 1024 * 1024;

    /// Verifies the server of a wss URL, as `connectTLSStream()` does.
    /// Without it, wss URLs fail, and ws URLs still work. The web needs it
    /// too, though the browser verifies the servers there.
    std::shared_ptr<TrustStore> trustStore;
};

/// WebSocket is a connection that carries messages in both directions, by the
/// WebSocket protocol (RFC 6455).
///
/// A socket connects asynchronously. Each `update()` advances the connection
/// and reads the messages that arrived, and the resulting events wait in a
/// queue until `pollEvent()` takes them:
/// ```cpp
/// socket->update();
/// while (auto event = socket->pollEvent()) {
///     switch (event->kind) {
///     case StreamEventKind::Connected:
///         (void)socket->sendText("hello");
///         break;
///     case StreamEventKind::Received:
///         onMessage(event->messageType, std::move(event->data));
///         break;
///     case StreamEventKind::Disconnected:
///         onClosed(event->closeCode, event->error.get());
///         break;
///     }
/// }
/// ```
///
/// A socket is not thread-safe. Use it from the thread that runs the game loop.
class POMDOG_EXPORT WebSocket {
public:
    WebSocket() noexcept;

    WebSocket(const WebSocket&) = delete;
    WebSocket& operator=(const WebSocket&) = delete;

    virtual ~WebSocket();

    /// Advances the connection, sends the bytes that wait to be sent, and
    /// queues events for what happened, such as received messages. Call it
    /// once per frame, before taking events.
    virtual void
    update() = 0;

    /// Takes the oldest queued event, or returns nullopt when none waits.
    [[nodiscard]] virtual std::optional<WebSocketEvent>
    pollEvent() = 0;

    /// Sends text as a text message.
    ///
    /// Fails when the socket is not connected, and when text is not valid
    /// UTF-8. A failure to send closes the socket, which then reports
    /// Disconnected with the error.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    sendText(std::string_view text) = 0;

    /// Sends data as a binary message.
    ///
    /// Fails when the socket is not connected. A failure to send closes the
    /// socket, which then reports Disconnected with the error.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    sendBinary(std::span<const u8> data) = 0;

    /// Starts the closing handshake with code and reason, such as 1000 and an
    /// empty reason for a normal closure.
    ///
    /// The socket stops reporting messages, and reports Disconnected once the
    /// server answers and closes, or with an error after
    /// `WebSocketOptions::connectTimeout`. While the connection opens, it
    /// stops opening and reports Disconnected with an error. Does nothing
    /// after the closing started.
    ///
    /// Fails, as the browser does, when code is neither 1000 nor from 3000 to
    /// 4999, and when reason is not valid UTF-8 or longer than 123 bytes.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    close(u16 code, std::string_view reason) = 0;

    /// Returns true if the socket is connected and neither side started
    /// closing, false otherwise.
    [[nodiscard]] virtual bool
    isConnected() const noexcept = 0;

    /// Returns the subprotocol that the server picked, or an empty string
    /// before Connected and when the client offered none.
    [[nodiscard]] virtual std::string_view
    getProtocol() const noexcept = 0;
};

/// Starts a WebSocket connection to url, such as "wss://example.com/chat".
///
/// The returned socket reports Connected, or Disconnected with an error, after
/// a later `WebSocket::update()`. Returns an error when url is not a ws or wss
/// URL, or has a fragment, user information, a space, or a control character,
/// and when the subprotocols are invalid. Also returns an error when url is
/// wss and the options have no trust store.
///
/// The handshake request has no headers but those of the protocol, since
/// browsers do not let a page add any. Authenticate with the URL, a
/// subprotocol, or the first message instead.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<WebSocket>, std::unique_ptr<Error>>
connectWebSocket(std::string_view url, const WebSocketOptions& options);

} // namespace pomdog::network
