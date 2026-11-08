// Copyright mogemimi. Distributed under the MIT license.

#include "tests/network/network_test_helper.h"
#include "tests/network/network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/web_socket.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getClosedTCPServerAddress;
using pomdog::testing::getHTTPServerAddress;
using pomdog::testing::getHTTPSServerAddress;

// NOTE: The tests talk to the WebSocket paths of the local HTTP and HTTPS
// servers of network-test-runner. On the web, Node.js connects with its
// WebSocket, which follows the same rules as the browsers.

namespace {

[[nodiscard]] std::string
localURL(std::string_view path)
{
    return "ws://" + std::string{getHTTPServerAddress()} + std::string{path};
}

[[nodiscard]] std::string
localSecureURL(std::string_view path)
{
    return "wss://" + std::string{getHTTPSServerAddress()} + std::string{path};
}

/// Starts connecting to url, and fails the test when it cannot start.
[[nodiscard]] std::shared_ptr<WebSocket>
startSocket(std::string_view url, const WebSocketOptions& options = WebSocketOptions{})
{
    auto socket = connectWebSocket(url, options);
    if (!socket) {
        FAIL("failed to start the WebSocket: " << describe(socket.error()));
    }
    REQUIRE(*socket != nullptr);
    return std::move(*socket);
}

/// Updates socket until an event arrives, and returns it.
[[nodiscard]] WebSocketEvent
waitForEvent(WebSocket& socket)
{
    std::optional<WebSocketEvent> received;
    const auto finished = runUntil([&] {
        socket.update();
        received = socket.pollEvent();
        return received.has_value();
    });
    REQUIRE(finished);
    REQUIRE(received.has_value());
    return std::move(*received);
}

/// Updates socket until it reports Connected. Fails the test on any other
/// event.
///
/// The socket may be closed already, when the server closed right after the
/// handshake, within the same `update()`.
void waitForOpen(WebSocket& socket)
{
    const auto event = waitForEvent(socket);
    if (event.kind != StreamEventKind::Connected) {
        FAIL("failed to connect: " << describe(event.error));
    }
}

/// Connects to url, and waits for Connected.
[[nodiscard]] std::shared_ptr<WebSocket>
openSocket(std::string_view url, const WebSocketOptions& options = WebSocketOptions{})
{
    auto socket = startSocket(url, options);
    waitForOpen(*socket);
    return socket;
}

/// Updates socket until a message arrives, and returns it. Fails the test on
/// any other event.
[[nodiscard]] WebSocketEvent
waitForMessage(WebSocket& socket)
{
    auto event = waitForEvent(socket);
    if (event.kind != StreamEventKind::Received) {
        FAIL("expected a message, but the socket closed: " << describe(event.error));
    }
    return event;
}

/// Updates socket until it reports Disconnected, and returns that event.
/// Fails the test on any other event.
[[nodiscard]] WebSocketEvent
waitForClose(WebSocket& socket)
{
    auto event = waitForEvent(socket);
    if (event.kind == StreamEventKind::Received) {
        FAIL("unexpected message: " << asText(event.data));
    }
    REQUIRE(event.kind == StreamEventKind::Disconnected);
    REQUIRE_FALSE(socket.isConnected());
    return event;
}

/// Connects to url, and returns the error of the failure. Fails the test when
/// the socket connects.
[[nodiscard]] std::unique_ptr<Error>
connectError(std::string_view url, const WebSocketOptions& options = WebSocketOptions{})
{
    auto socket = startSocket(url, options);
    auto event = waitForEvent(*socket);
    if (event.kind != StreamEventKind::Disconnected) {
        FAIL("the socket connected, but it should fail");
    }
    REQUIRE(event.error != nullptr);
    REQUIRE(event.closeCode == 1006);
    REQUIRE_FALSE(socket->isConnected());
    return std::move(event.error);
}

/// Waits for the error that closes socket, after an optional Connected event.
[[nodiscard]] std::unique_ptr<Error>
waitForFailure(WebSocket& socket)
{
    auto event = waitForEvent(socket);
    if (event.kind == StreamEventKind::Connected) {
        event = waitForEvent(socket);
    }
    if (event.kind == StreamEventKind::Received) {
        FAIL("unexpected message: " << asText(event.data));
    }
    REQUIRE(event.kind == StreamEventKind::Disconnected);
    REQUIRE(event.error != nullptr);
    REQUIRE(event.closeCode == 1006);
    return std::move(event.error);
}

[[nodiscard]] std::vector<u8>
toBytes(std::string_view text)
{
    const auto bytes = asBytes(text);
    return std::vector<u8>(bytes.begin(), bytes.end());
}

} // namespace

TEST_CASE("WebSocket echoes text and binary messages")
{
    auto socket = openSocket(localURL("/ws/echo"));
    REQUIRE(socket->isConnected());
    REQUIRE(socket->getProtocol().empty());

    REQUIRE(socket->sendText("hello").has_value());
    auto event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Text);
    REQUIRE(asText(event.data) == "hello");

    std::vector<u8> bytes;
    for (int i = 0; i < 256; ++i) {
        bytes.push_back(static_cast<u8>(i));
    }
    REQUIRE(socket->sendBinary(bytes).has_value());
    event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Binary);
    REQUIRE(event.data == bytes);

    REQUIRE(socket->sendText("").has_value());
    REQUIRE(socket->sendBinary({}).has_value());
    event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Text);
    REQUIRE(event.data.empty());
    event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Binary);
    REQUIRE(event.data.empty());

    // NOTE: Text may hold null characters.
    const std::string_view withNull{"a\0b", 3};
    REQUIRE(socket->sendText(withNull).has_value());
    event = waitForMessage(*socket);
    REQUIRE(asText(event.data) == withNull);
}

TEST_CASE("WebSocket keeps the order of messages")
{
    auto socket = openSocket(localURL("/ws/echo"));
    for (int i = 0; i < 10; ++i) {
        REQUIRE(socket->sendText("message " + std::to_string(i)).has_value());
    }
    for (int i = 0; i < 10; ++i) {
        const auto event = waitForMessage(*socket);
        REQUIRE(asText(event.data) == "message " + std::to_string(i));
    }
}

TEST_CASE("WebSocket drops the byte order mark of a received text message")
{
    // NOTE: Browsers decode text messages without the byte order mark, so the
    // native client drops it too. A sent message keeps it.
    auto socket = openSocket(localURL("/ws/echo"));
    REQUIRE(socket->sendText("\xef\xbb\xbfhello").has_value());
    const auto event = waitForMessage(*socket);
    REQUIRE(asText(event.data) == "hello");
}

TEST_CASE("WebSocket sends and receives large messages")
{
    auto socket = openSocket(localURL("/ws/bytes/1000000"));
    auto event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Binary);
    REQUIRE(asText(event.data) == makeLetters(1000000));

    const auto text = makeLetters(300000);
    REQUIRE(socket->sendText(text).has_value());
    event = waitForMessage(*socket);
    REQUIRE(asText(event.data) == text);
}

TEST_CASE("WebSocket picks the subprotocol that the server answers")
{
    WebSocketOptions options;
    options.protocols = {"chat", "superchat"};
    auto socket = openSocket(localURL("/ws/echo"), options);
    REQUIRE(socket->getProtocol() == "chat");

    socket = openSocket(localURL("/ws/echo?protocol=superchat"), options);
    REQUIRE(socket->getProtocol() == "superchat");
}

TEST_CASE("WebSocket fails when the server picks a subprotocol that the client did not offer")
{
    WebSocketOptions options;
    options.protocols = {"chat"};
    [[maybe_unused]] auto err = connectError(localURL("/ws/echo?protocol=other"), options);
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    REQUIRE_MESSAGE(err->toString().find("did not offer") != std::string::npos, describe(err));

    // NOTE: The WebSocket of Node.js 24 throws an uncaught TypeError, which
    // ends the process, when the client offered no subprotocol and the server
    // picks one. Browsers fail the connection.
    err = connectError(localURL("/ws/echo?protocol=chat"));
#endif
}

TEST_CASE("WebSocket fails when the server picks none of the subprotocols that the client offered")
{
    WebSocketOptions options;
    options.protocols = {"chat"};

    for (const auto path : {"/ws/echo?protocol=none", "/ws/echo?protocol="}) {
        [[maybe_unused]] const auto err = connectError(localURL(path), options);
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
        REQUIRE_MESSAGE(err->toString().find("picked none") != std::string::npos, describe(err));
#endif
    }
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
TEST_CASE("WebSocket closes with an error when too many bytes wait to be sent")
{
    auto socket = openSocket(localURL("/ws/echo"));

    // NOTE: The server stops reading while it cannot send an echo, and the
    // socket is not updated, so it reads no echo. A socket without the limit
    // would use up the memory, so the loop ends at a size that the local
    // connection cannot explain.
    const auto message = toBytes(makeLetters(1024 * 1024));
    bool failed = false;
    for (std::size_t size = 0; !failed && (size < (pomdog::network::detail::maxPendingSendSize * 2)); size += message.size()) {
        failed = !socket->sendBinary(message).has_value();
    }
    REQUIRE(failed);

    const auto event = waitForClose(*socket);
    REQUIRE(event.closeCode == 1006);
    REQUIRE(event.error != nullptr);
    REQUIRE_MESSAGE(event.error->toString().find("wait to be sent") != std::string::npos, describe(event.error));
}
#endif

TEST_CASE("WebSocket closes with the closing handshake")
{
    auto socket = openSocket(localURL("/ws/echo"));
    REQUIRE(socket->close(4000, "bye").has_value());
    REQUIRE_FALSE(socket->isConnected());
    REQUIRE_FALSE(socket->sendText("after close").has_value());

    const auto event = waitForClose(*socket);
    REQUIRE_MESSAGE(event.error == nullptr, describe(event.error));
    REQUIRE(event.closeCode == 4000);
    REQUIRE(event.closeReason == "bye");

    // NOTE: A second close() does nothing.
    REQUIRE(socket->close(1000, "").has_value());
    socket->update();
    REQUIRE_FALSE(socket->pollEvent().has_value());
}

TEST_CASE("WebSocket reports the Close frame of the server")
{
    auto socket = openSocket(localURL("/ws/close?code=4001&reason=done"));
    auto event = waitForClose(*socket);
    REQUIRE_MESSAGE(event.error == nullptr, describe(event.error));
    REQUIRE(event.closeCode == 4001);
    REQUIRE(event.closeReason == "done");

    socket = openSocket(localURL("/ws/close"));
    event = waitForClose(*socket);
    REQUIRE_MESSAGE(event.error == nullptr, describe(event.error));
    REQUIRE(event.closeCode == 1005);
    REQUIRE(event.closeReason.empty());
}

TEST_CASE("WebSocket reports an error when the server closes without a Close frame")
{
    auto socket = openSocket(localURL("/ws/drop"));
    const auto event = waitForClose(*socket);
    REQUIRE(event.error != nullptr);
    REQUIRE(event.closeCode == 1006);
}

TEST_CASE("WebSocket answers a Ping with a Pong")
{
    auto socket = openSocket(localURL("/ws/ping"));
    const auto event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Text);
    REQUIRE(asText(event.data) == "pong:hello");
}

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
TEST_CASE("WebSocket answers only the last of the Pings that one update reads")
{
    constexpr int pingCount = 1000;
    auto socket = openSocket(localURL("/ws/ping-burst/" + std::to_string(pingCount)));
    REQUIRE(socket->sendText("start").has_value());

    // NOTE: The Pings arrive while the socket is not updated. A busy machine
    // may split them over a few updates, so the test allows a few Pongs.
    std::this_thread::sleep_for(std::chrono::milliseconds{200});

    const auto event = waitForMessage(*socket);
    const auto pongCount = std::stoi(std::string{asText(event.data)});
    REQUIRE(pongCount >= 1);
    REQUIRE(pongCount < pingCount);
}
#endif

TEST_CASE("WebSocket puts the fragments of a message together")
{
    auto socket = openSocket(localURL("/ws/fragments"));
    const auto event = waitForMessage(*socket);
    REQUIRE(event.messageType == WebSocketMessageType::Text);
    REQUIRE(asText(event.data) == "hello, world");
}

TEST_CASE("WebSocket fails a message larger than maxMessageSize")
{
    WebSocketOptions options;
    options.maxMessageSize = 1000;

    auto socket = openSocket(localURL("/ws/bytes/1000"), options);
    REQUIRE(waitForMessage(*socket).data.size() == 1000);

    socket = openSocket(localURL("/ws/bytes/1001"), options);
    [[maybe_unused]] const auto err = waitForFailure(*socket);
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    REQUIRE_MESSAGE(err->toString().find("larger than 1000 bytes") != std::string::npos, describe(err));
#endif
}

TEST_CASE("WebSocket fails frames that break the protocol")
{
    for (const auto kind : std::to_array<std::string_view>({"masked", "reserved", "opcode", "utf8", "continuation", "close-code", "fragmented-ping"})) {
        CAPTURE(kind);
        auto socket = startSocket(localURL("/ws/bad/" + std::string{kind}));
        [[maybe_unused]] const auto err = waitForFailure(*socket);
    }
}

TEST_CASE("WebSocket fails a handshake that the server answers wrongly")
{
    for (const auto path : std::to_array<std::string_view>({"/ws/handshake/accept", "/status/404", "/redirect/302?location=/ws/echo"})) {
        CAPTURE(path);
        [[maybe_unused]] const auto err = connectError(localURL(path));
    }

#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    auto err = connectError(localURL("/status/404"));
    REQUIRE_MESSAGE(err->toString().find("status 404") != std::string::npos, describe(err));

    // NOTE: The native client offers no extension, so it fails a server that
    // accepts one. Browsers and Node.js offer permessage-deflate, and accept
    // the same answer.
    err = connectError(localURL("/ws/handshake/extension"));
    REQUIRE_MESSAGE(err->toString().find("permessage-deflate") != std::string::npos, describe(err));
#endif
}

TEST_CASE("WebSocket fails a handshake that takes longer than connectTimeout")
{
    WebSocketOptions options;
    options.connectTimeout = std::chrono::milliseconds{200};

    const auto startTime = std::chrono::steady_clock::now();
    [[maybe_unused]] const auto err = connectError(localURL("/delay/3000"), options);
    REQUIRE((std::chrono::steady_clock::now() - startTime) < std::chrono::seconds{2});
#if !defined(POMDOG_PLATFORM_EMSCRIPTEN)
    REQUIRE_MESSAGE(err->toString().find("did not finish within 200 ms") != std::string::npos, describe(err));
#endif
}

TEST_CASE("WebSocket fails to connect to an address where nothing listens")
{
    [[maybe_unused]] const auto err = connectError("ws://" + std::string{getClosedTCPServerAddress()} + "/");
}

TEST_CASE("WebSocket fails a close() that the server does not answer")
{
    WebSocketOptions options;
    options.connectTimeout = std::chrono::milliseconds{300};

    auto socket = openSocket(localURL("/ws/silent"), options);
    REQUIRE(socket->close(1000, "").has_value());

    const auto startTime = std::chrono::steady_clock::now();
    const auto event = waitForClose(*socket);
    REQUIRE((std::chrono::steady_clock::now() - startTime) < std::chrono::seconds{2});
    REQUIRE(event.error != nullptr);
    REQUIRE(event.closeCode == 1006);
}

TEST_CASE("WebSocket stops connecting at close()")
{
    auto socket = startSocket(localURL("/ws/echo"));
    REQUIRE(socket->close(1000, "").has_value());

    auto event = socket->pollEvent();
    REQUIRE(event.has_value());
    REQUIRE(event->kind == StreamEventKind::Disconnected);
    REQUIRE(event->error != nullptr);
    REQUIRE(event->closeCode == 1006);

    // NOTE: No Connected event follows, though the server would accept.
    bool hasEvent = false;
    const auto startTime = std::chrono::steady_clock::now();
    const auto finished = runUntil([&] {
        socket->update();
        hasEvent = hasEvent || socket->pollEvent().has_value();
        return (std::chrono::steady_clock::now() - startTime) > std::chrono::milliseconds{300};
    });
    REQUIRE(finished);
    REQUIRE_FALSE(hasEvent);
}

TEST_CASE("WebSocket fails sends and closes that the browser refuses")
{
    auto socket = startSocket(localURL("/ws/echo"));
    REQUIRE_FALSE(socket->sendText("too early").has_value());
    REQUIRE_FALSE(socket->sendBinary(toBytes("too early")).has_value());
    waitForOpen(*socket);
    REQUIRE(socket->isConnected());

    REQUIRE_FALSE(socket->sendText("\xff").has_value());
    REQUIRE_FALSE(socket->close(1001, "").has_value());
    REQUIRE_FALSE(socket->close(1000, std::string(124, 'r')).has_value());
    REQUIRE_FALSE(socket->close(1000, "\xff").has_value());
    REQUIRE(socket->isConnected());

    REQUIRE(socket->sendText("still open").has_value());
    REQUIRE(asText(waitForMessage(*socket).data) == "still open");
}

TEST_CASE("connectWebSocket fails URLs and subprotocols that the browser refuses")
{
    REQUIRE_FALSE(connectWebSocket("http://" + std::string{getHTTPServerAddress()} + "/ws/echo", WebSocketOptions{}).has_value());
    REQUIRE_FALSE(connectWebSocket(localURL("/ws/echo#fragment"), WebSocketOptions{}).has_value());

    WebSocketOptions options;
    options.protocols = {"two words"};
    REQUIRE_FALSE(connectWebSocket(localURL("/ws/echo"), options).has_value());
    options.protocols = {"chat", "chat"};
    REQUIRE_FALSE(connectWebSocket(localURL("/ws/echo"), options).has_value());
}

TEST_CASE("WebSocket connects over TLS")
{
    WebSocketOptions options;
    options.trustStore = loadTestTrustStore();
    auto socket = openSocket(localSecureURL("/ws/echo"), options);
    REQUIRE(socket->sendText("secure").has_value());
    REQUIRE(asText(waitForMessage(*socket).data) == "secure");

    REQUIRE(socket->close(1000, "").has_value());
    const auto event = waitForClose(*socket);
    REQUIRE_MESSAGE(event.error == nullptr, describe(event.error));
    REQUIRE(event.closeCode == 1000);
}

TEST_CASE("WebSocket fails wss without a trust store and with a wrong host")
{
    REQUIRE_FALSE(connectWebSocket(localSecureURL("/ws/echo"), WebSocketOptions{}).has_value());

    // NOTE: The certificate of the server names only 127.0.0.1.
    const std::string_view address = getHTTPSServerAddress();
    const auto port = address.substr(address.rfind(':'));
    WebSocketOptions options;
    options.trustStore = loadTestTrustStore();
    [[maybe_unused]] const auto err = connectError("wss://localhost" + std::string{port} + "/ws/echo", options);
}
