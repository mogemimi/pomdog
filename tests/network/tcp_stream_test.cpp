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
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using namespace pomdog;
using namespace pomdog::network;
using pomdog::testing::getClosedTCPServerAddress;
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

TEST_CASE("TCPStream sends a large write")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    waitForConnected(*stream);

    const auto line = makeLetters(40000) + "\n";
    REQUIRE(stream->write(asBytes(line)).has_value());
    REQUIRE(receiveText(*stream, line.size()) == line);
}

TEST_CASE("TCPStream sends a write larger than a local connection holds")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    waitForConnected(*stream);

    const auto text = makeNumberedLines();
    REQUIRE(stream->write(asBytes(text)).has_value());
    const bool isSame = (receiveText(*stream, text.size()) == text);
    REQUIRE(isSame);
}

TEST_CASE("TCPStream reports a failed send as Disconnected with an error")
{
    auto result = connectTCPStream(getTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    waitForConnected(*stream);

    REQUIRE(stream->write(asBytes("bye\n")).has_value());

    // NOTE: The stream is not updated, so it does not read the answer or that
    // the server closed the connection. One update can read both.
    writeUntilSendFails(*stream);
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

TEST_CASE("TCPStream reads the bytes that arrived during a pause longer than the idle timeout")
{
    StreamOptions options;
    options.idleTimeout = std::chrono::milliseconds{200};

    auto result = connectTCPStream(getTCPServerAddress(), options);
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    waitForConnected(*stream);
    REQUIRE(stream->write(asBytes("hello\n")).has_value());

    // NOTE: The echo arrives while the game calls no update(), such as during
    // a long load.
    std::this_thread::sleep_for(std::chrono::milliseconds{400});
    REQUIRE(receiveText(*stream, 6) == "hello\n");
    REQUIRE(stream->isConnected());
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

TEST_CASE("TCPStream reports a refused connection as Disconnected with an error")
{
    auto result = connectTCPStream(getClosedTCPServerAddress(), StreamOptions{});
    REQUIRE(result.has_value());
    auto stream = std::move(*result);

    // NOTE: waitForDisconnected() fails the test if Connected arrives first.
    const auto err = waitForDisconnected(*stream);
    REQUIRE(err != nullptr);
}

TEST_CASE("TCPStream reports a connect timeout as Disconnected with an error")
{
    // NOTE: 192.0.2.1 is in TEST-NET-1, which no network routes. Depending on
    // the network, the connection times out or fails as unreachable, and both
    // are errors.
    StreamOptions options;
    options.connectTimeout = std::chrono::milliseconds{200};

    auto result = connectTCPStream("192.0.2.1:9", options);
    REQUIRE(result.has_value());
    auto stream = std::move(*result);

    const auto startTime = std::chrono::steady_clock::now();
    const auto err = waitForDisconnected(*stream);
    REQUIRE(err != nullptr);

    // NOTE: A generous bound, which only a connect that ignores the timeout
    // exceeds.
    REQUIRE((std::chrono::steady_clock::now() - startTime) < std::chrono::seconds{10});
}

TEST_CASE("TCPStream that is destroyed while connecting returns at once")
{
    // NOTE: 192.0.2.1 (TEST-NET-1) never answers, so the connection would run
    // until the timeout.
    StreamOptions options;
    options.connectTimeout = std::chrono::seconds{10};

    auto result = connectTCPStream("192.0.2.1:9", options);
    REQUIRE(result.has_value());
    auto stream = std::move(*result);
    stream->update();

    const auto startTime = std::chrono::steady_clock::now();
    stream.reset();
    REQUIRE((std::chrono::steady_clock::now() - startTime) < std::chrono::seconds{2});
}
