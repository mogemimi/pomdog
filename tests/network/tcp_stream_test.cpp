// Copyright mogemimi. Distributed under the MIT license.

#include "network_test_helper.h"
#include "network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <chrono>
#include <ostream>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getTCPServerAddress;

// NOTE: The TCP server echoes each line, and closes the connection after it
// echoes "bye\n".

TEST_CASE("TCPStream echoes lines through the local server")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    REQUIRE_FALSE(stream->isConnected());

    bool connected = false;
    std::string received;
    const auto finished = runUntil([&] {
        stream->update();
        while (auto event = stream->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                REQUIRE_FALSE(connected);
                connected = true;
                REQUIRE(stream->isConnected());
                REQUIRE(stream->write(asBytes("hello\n")).has_value());
                REQUIRE(stream->write(asBytes("world\n")).has_value());
            }
            else if (event->kind == StreamEventKind::Received) {
                received += asText(event->data);
            }
            else {
                FAIL("unexpected disconnection: " << describe(event->error));
            }
        }
        return received.size() >= 12;
    });
    REQUIRE(finished);
    REQUIRE(connected);
    REQUIRE(received == "hello\nworld\n");

    stream->close();
    REQUIRE_FALSE(stream->isConnected());

    // NOTE: A closed stream reports no events and cannot send.
    stream->update();
    REQUIRE_FALSE(stream->pollEvent().has_value());
    REQUIRE_FALSE(stream->write(asBytes("hello\n")).has_value());
}

TEST_CASE("TCPStream reports Disconnected without an error when the server closes the connection")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);

    std::string received;
    bool disconnected = false;
    const auto finished = runUntil([&] {
        stream->update();
        while (auto event = stream->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                REQUIRE(stream->write(asBytes("bye\n")).has_value());
            }
            else if (event->kind == StreamEventKind::Received) {
                REQUIRE_FALSE(disconnected);
                received += asText(event->data);
            }
            else {
                REQUIRE(event->error == nullptr);
                disconnected = true;
            }
        }
        return disconnected;
    });
    REQUIRE(finished);
    REQUIRE(received == "bye\n");
    REQUIRE_FALSE(stream->isConnected());

    stream->update();
    REQUIRE_FALSE(stream->pollEvent().has_value());
}

TEST_CASE("TCPStream reports Disconnected with an error after the idle timeout")
{
    StreamOptions options;
    options.idleTimeout = std::chrono::milliseconds{200};

    auto result = connectTCPStream(getTCPServerAddress(), options);
    REQUIRE(result.has_value());
    auto stream = std::move(*result);

    // NOTE: The server sends nothing until it reads a line.
    bool connected = false;
    bool disconnected = false;
    const auto finished = runUntil([&] {
        stream->update();
        while (auto event = stream->pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                connected = true;
            }
            else if (event->kind == StreamEventKind::Received) {
                FAIL("unexpected data: " << asText(event->data));
            }
            else {
                REQUIRE(event->error != nullptr);
                disconnected = true;
            }
        }
        return disconnected;
    });
    REQUIRE(finished);
    REQUIRE(connected);
    REQUIRE_FALSE(stream->isConnected());
}

TEST_CASE("TCPStream that is closed while connecting reports no events")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);

    stream->close();
    REQUIRE_FALSE(stream->isConnected());

    for (int i = 0; i < 10; ++i) {
        stream->update();
        REQUIRE_FALSE(stream->pollEvent().has_value());
    }
    REQUIRE_FALSE(stream->write(asBytes("hello\n")).has_value());
}
