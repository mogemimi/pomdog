// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "tests/network/network_test_options.h"
#include "tests/testing/testing.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/trust_store.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <iterator>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
#include <emscripten.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in the helpers below.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4866)

namespace pomdog::network {

/// Returns the bytes of text.
[[nodiscard]] inline std::span<const u8>
asBytes(std::string_view text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns bytes as text.
[[nodiscard]] inline std::string_view
asText(const std::vector<u8>& bytes) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns the message of err, or "no error" when err is null.
[[nodiscard]] inline std::string
describe(const std::unique_ptr<Error>& err)
{
    return (err != nullptr) ? err->toString() : std::string{"no error"};
}

/// Returns size bytes that repeat the letters from a to z.
[[nodiscard]] inline std::string
makeLetters(std::size_t size)
{
    std::string text;
    text.reserve(size);
    for (std::size_t i = 0; i < size; ++i) {
        text.push_back(static_cast<char>('a' + (i % 26)));
    }
    return text;
}

/// Returns the root of the local TLS servers in PEM format, which `--ca-file`
/// names.
[[nodiscard]] inline std::string
readTestRootCertificate()
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    // NOTE: The store holds nothing on the web, where Node.js verifies the
    // servers with the root that NODE_EXTRA_CA_CERTS names. The runner sets it.
    return {};
#else
    const auto path = std::string{pomdog::testing::getCAFile()};
    std::ifstream file{path, std::ios::binary};
    REQUIRE_MESSAGE(file.is_open(), "failed to open " << path);
    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
#endif
}

/// Returns a trust store that holds the root of the local TLS servers, and
/// distrustedKeys, a `.dkeys` file or empty.
[[nodiscard]] inline std::shared_ptr<TrustStore>
loadTestTrustStore(std::span<const u8> distrustedKeys = {})
{
    const auto pem = readTestRootCertificate();

    TrustStoreDesc desc;
    desc.rootCertificatesPEM = asBytes(pem);
    desc.distrustedKeys = distrustedKeys;
    auto store = createTrustStore(desc);
    if (!store) {
        FAIL("failed to create the trust store: " << describe(store.error()));
    }
    REQUIRE(*store != nullptr);
    return std::move(*store);
}

/// Sleeps for about a millisecond, like the gap between two frames. On the
/// web it returns to the event loop meanwhile, so that fetch() can progress.
inline void
sleepBriefly()
{
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
    emscripten_sleep(1);
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
}

/// Calls step about once per millisecond until it returns true, like a game
/// loop that updates network objects every frame. Returns false when that
/// takes longer than timeout, which is longer than the network timeouts that
/// the tests use.
[[nodiscard]] inline bool
runUntil(const std::function<bool()>& step, std::chrono::seconds timeout = std::chrono::seconds{30})
{
    const auto startTime = std::chrono::steady_clock::now();
    while (!step()) {
        if ((std::chrono::steady_clock::now() - startTime) > timeout) {
            return false;
        }
        sleepBriefly();
    }
    return true;
}

/// Updates stream until it reports Connected. Fails the test on any other
/// event.
inline void
waitForConnected(Stream& stream)
{
    bool connected = false;
    const auto finished = runUntil([&] {
        stream.update();
        while (auto event = stream.pollEvent()) {
            if (event->kind == StreamEventKind::Connected) {
                connected = true;
            }
            else if (event->kind == StreamEventKind::Received) {
                FAIL("unexpected data: " << asText(event->data));
            }
            else {
                FAIL("failed to connect: " << describe(event->error));
            }
        }
        return connected;
    });
    REQUIRE(finished);
    REQUIRE(stream.isConnected());
}

/// Updates stream until it has received size bytes, and returns them. Fails
/// the test on any other event before that, and leaves later events in the
/// queue, such as the Disconnected event of a server that closes after it
/// sends.
[[nodiscard]] inline std::string
receiveText(Stream& stream, std::size_t size)
{
    std::string received;
    const auto finished = runUntil([&] {
        stream.update();
        while (received.size() < size) {
            auto event = stream.pollEvent();
            if (!event) {
                break;
            }
            if (event->kind == StreamEventKind::Received) {
                received += asText(event->data);
            }
            else if (event->kind == StreamEventKind::Disconnected) {
                FAIL("unexpected disconnection: " << describe(event->error));
            }
            else {
                FAIL("unexpected Connected event");
            }
        }
        return received.size() >= size;
    });
    REQUIRE(finished);
    return received;
}

/// Updates stream until it reports Disconnected, and returns the error of that
/// event. Fails the test on any other event.
[[nodiscard]] inline std::unique_ptr<Error>
waitForDisconnected(Stream& stream)
{
    std::unique_ptr<Error> err;
    bool disconnected = false;
    const auto finished = runUntil([&] {
        stream.update();
        while (auto event = stream.pollEvent()) {
            if (event->kind == StreamEventKind::Disconnected) {
                err = std::move(event->error);
                disconnected = true;
            }
            else if (event->kind == StreamEventKind::Received) {
                FAIL("unexpected data: " << asText(event->data));
            }
            else {
                FAIL("unexpected Connected event");
            }
        }
        return disconnected;
    });
    REQUIRE(finished);
    REQUIRE_FALSE(stream.isConnected());
    return err;
}

/// Writes to stream, without updating it, until a send fails and closes the
/// stream, which then reports Disconnected with an error. The server must have
/// closed the connection, so that it resets the connection when bytes arrive.
inline void
writeUntilSendFails(Stream& stream)
{
    const auto finished = runUntil([&] {
        REQUIRE(stream.write(asBytes("hello\n")).has_value());
        return !stream.isConnected();
    });
    REQUIRE(finished);

    auto event = stream.pollEvent();
    REQUIRE(event.has_value());
    REQUIRE(event->kind == StreamEventKind::Disconnected);
    REQUIRE(event->error != nullptr);
    REQUIRE_FALSE(stream.pollEvent().has_value());
    REQUIRE_FALSE(stream.write(asBytes("hello\n")).has_value());
}

} // namespace pomdog::network

POMDOG_MSVC_SUPPRESS_WARNING_POP
