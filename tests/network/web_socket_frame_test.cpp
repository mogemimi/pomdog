// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/web_socket_frame.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

// NOTE: Suppress C4866 warning for doctest expressions in this file
POMDOG_MSVC_SUPPRESS_WARNING(4866)

using pomdog::u16;
using pomdog::u64;
using pomdog::u8;
using pomdog::network::detail::encodeWebSocketFrame;
using pomdog::network::detail::makeWebSocketClosePayload;
using pomdog::network::detail::WebSocketMaskKey;
using pomdog::network::detail::WebSocketMessage;
using pomdog::network::detail::WebSocketOpcode;
using pomdog::network::detail::WebSocketReader;

namespace {

[[nodiscard]] std::vector<u8>
toBytes(std::string_view text)
{
    std::vector<u8> bytes;
    for (const char c : text) {
        bytes.push_back(static_cast<u8>(c));
    }
    return bytes;
}

[[nodiscard]] std::string
toText(const std::vector<u8>& bytes)
{
    std::string text;
    for (const u8 b : bytes) {
        text += static_cast<char>(b);
    }
    return text;
}

/// Returns a frame that a server sends, without a mask.
[[nodiscard]] std::vector<u8>
makeServerFrame(bool fin, u8 opcode, std::string_view payload)
{
    std::vector<u8> frame;
    frame.push_back(static_cast<u8>((fin ? 0x80 : 0x00) | opcode));
    if (payload.size() < 126) {
        frame.push_back(static_cast<u8>(payload.size()));
    }
    else if (payload.size() <= 0xffff) {
        frame.push_back(126);
        frame.push_back(static_cast<u8>(payload.size() >> 8));
        frame.push_back(static_cast<u8>(payload.size() & 0xff));
    }
    else {
        frame.push_back(127);
        for (int shift = 56; shift >= 0; shift -= 8) {
            frame.push_back(static_cast<u8>((static_cast<u64>(payload.size()) >> shift) & 0xff));
        }
    }
    const auto bytes = toBytes(payload);
    frame.insert(frame.end(), bytes.begin(), bytes.end());
    return frame;
}

/// Reads frames with a new reader, and returns the first message.
[[nodiscard]] WebSocketMessage
readOne(const std::vector<u8>& frames, std::size_t maxMessageSize = 0)
{
    WebSocketReader reader{maxMessageSize};
    reader.append(frames);
    auto result = reader.next();
    if (!result) {
        FAIL("the reader failed: " << result.error().error->toString());
    }
    REQUIRE(result->has_value());
    return std::move(**result);
}

/// Reads frames with a new reader, and returns the close code of its
/// failure, or zero when it does not fail.
[[nodiscard]] u16
readFailure(const std::vector<u8>& frames, std::size_t maxMessageSize = 0)
{
    WebSocketReader reader{maxMessageSize};
    reader.append(frames);
    for (;;) {
        auto result = reader.next();
        if (!result) {
            REQUIRE(result.error().error != nullptr);
            return result.error().closeCode;
        }
        if (!result->has_value()) {
            return 0;
        }
    }
}

[[nodiscard]] std::vector<u8>
concat(std::initializer_list<std::vector<u8>> parts)
{
    std::vector<u8> result;
    for (const auto& part : parts) {
        result.insert(result.end(), part.begin(), part.end());
    }
    return result;
}

} // namespace

TEST_CASE("encodeWebSocketFrame masks the payload of a client frame")
{
    // NOTE: The example of RFC 6455, section 5.7: a masked text frame "Hello".
    const WebSocketMaskKey maskKey = {0x37, 0xfa, 0x21, 0x3d};
    const auto frame = encodeWebSocketFrame(WebSocketOpcode::Text, toBytes("Hello"), maskKey);
    REQUIRE(frame == std::vector<u8>{0x81, 0x85, 0x37, 0xfa, 0x21, 0x3d, 0x7f, 0x9f, 0x4d, 0x51, 0x58});
}

TEST_CASE("encodeWebSocketFrame writes 16-bit and 64-bit lengths")
{
    const WebSocketMaskKey maskKey = {0, 0, 0, 0};

    auto frame = encodeWebSocketFrame(WebSocketOpcode::Binary, std::vector<u8>(125), maskKey);
    REQUIRE(frame.size() == 2 + 4 + 125);
    REQUIRE(frame[1] == (0x80 | 125));

    frame = encodeWebSocketFrame(WebSocketOpcode::Binary, std::vector<u8>(126), maskKey);
    REQUIRE(frame.size() == 4 + 4 + 126);
    REQUIRE(frame[1] == (0x80 | 126));
    REQUIRE(frame[2] == 0x00);
    REQUIRE(frame[3] == 126);

    frame = encodeWebSocketFrame(WebSocketOpcode::Binary, std::vector<u8>(65536), maskKey);
    REQUIRE(frame.size() == 10 + 4 + 65536);
    REQUIRE(frame[1] == (0x80 | 127));
    REQUIRE(std::vector<u8>(frame.begin() + 2, frame.begin() + 10) == std::vector<u8>{0, 0, 0, 0, 0, 1, 0, 0});
}

TEST_CASE("makeWebSocketClosePayload holds the code and the reason")
{
    REQUIRE(makeWebSocketClosePayload(1000, "bye") == std::vector<u8>{0x03, 0xe8, 'b', 'y', 'e'});
    REQUIRE(makeWebSocketClosePayload(4000, "") == std::vector<u8>{0x0f, 0xa0});
    REQUIRE(makeWebSocketClosePayload(1005, "ignored").empty());
}

TEST_CASE("WebSocketReader reads text and binary messages")
{
    auto message = readOne(makeServerFrame(true, 0x1, "Hello"));
    REQUIRE(message.opcode == WebSocketOpcode::Text);
    REQUIRE(toText(message.payload) == "Hello");

    message = readOne(makeServerFrame(true, 0x2, std::string(300, 'x')));
    REQUIRE(message.opcode == WebSocketOpcode::Binary);
    REQUIRE(message.payload.size() == 300);

    message = readOne(makeServerFrame(true, 0x2, std::string(70000, 'y')));
    REQUIRE(message.payload.size() == 70000);

    message = readOne(makeServerFrame(true, 0x1, ""));
    REQUIRE(message.opcode == WebSocketOpcode::Text);
    REQUIRE(message.payload.empty());
}

TEST_CASE("WebSocketReader waits for the rest of a frame")
{
    const auto frames = concat({makeServerFrame(true, 0x1, std::string(200, 'a')), makeServerFrame(true, 0x2, "b")});

    WebSocketReader reader{0};
    std::vector<WebSocketMessage> messages;
    for (const u8 b : frames) {
        POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
        reader.append(std::span<const u8>{&b, 1});
        POMDOG_CLANG_UNSAFE_BUFFER_END
        auto result = reader.next();
        REQUIRE(result.has_value());
        if (result->has_value()) {
            messages.push_back(std::move(**result));
        }
    }
    REQUIRE(messages.size() == 2);
    REQUIRE(messages[0].payload.size() == 200);
    REQUIRE(toText(messages[1].payload) == "b");
}

TEST_CASE("WebSocketReader puts fragments together around control frames")
{
    const auto frames = concat({
        makeServerFrame(false, 0x1, "Hel"),
        makeServerFrame(true, 0x9, "ping"),
        makeServerFrame(false, 0x0, "lo, "),
        makeServerFrame(true, 0x0, "world"),
    });

    WebSocketReader reader{0};
    reader.append(frames);

    auto result = reader.next();
    REQUIRE(result.has_value());
    REQUIRE(result->has_value());
    REQUIRE((*result)->opcode == WebSocketOpcode::Ping);
    REQUIRE(toText((*result)->payload) == "ping");

    result = reader.next();
    REQUIRE(result.has_value());
    REQUIRE(result->has_value());
    REQUIRE((*result)->opcode == WebSocketOpcode::Text);
    REQUIRE(toText((*result)->payload) == "Hello, world");

    result = reader.next();
    REQUIRE(result.has_value());
    REQUIRE_FALSE(result->has_value());
}

TEST_CASE("WebSocketReader reads the code and the reason of a Close frame")
{
    auto message = readOne(makeServerFrame(true, 0x8, std::string{"\x0f\xa1", 2} + "done"));
    REQUIRE(message.opcode == WebSocketOpcode::Close);
    REQUIRE(message.closeCode == 4001);
    REQUIRE(toText(message.payload) == "done");

    message = readOne(makeServerFrame(true, 0x8, ""));
    REQUIRE(message.closeCode == 1005);
    REQUIRE(message.payload.empty());
}

TEST_CASE("WebSocketReader fails frames that break the protocol with 1002")
{
    // NOTE: A server must not mask its frames.
    REQUIRE(readFailure({0x81, 0x82, 0, 0, 0, 0, 'h', 'i'}) == 1002);

    // NOTE: No extension was negotiated, so the reserved bits must be zero.
    REQUIRE(readFailure({0xc1, 0x02, 'h', 'i'}) == 1002);
    REQUIRE(readFailure({0xa1, 0x02, 'h', 'i'}) == 1002);
    REQUIRE(readFailure({0x91, 0x02, 'h', 'i'}) == 1002);

    // NOTE: Opcodes 3 to 7 and 11 to 15 are reserved.
    REQUIRE(readFailure({0x83, 0x00}) == 1002);
    REQUIRE(readFailure({0x8b, 0x00}) == 1002);

    REQUIRE(readFailure(makeServerFrame(false, 0x9, "hi")) == 1002);
    REQUIRE(readFailure(makeServerFrame(true, 0x9, std::string(126, 'p'))) == 1002);
    REQUIRE(readFailure(makeServerFrame(true, 0x0, "hi")) == 1002);
    REQUIRE(readFailure(concat({makeServerFrame(false, 0x1, "a"), makeServerFrame(true, 0x1, "b")})) == 1002);

    REQUIRE(readFailure({0x82, 0x7f, 0x80, 0, 0, 0, 0, 0, 0, 0}) == 1002);
}

TEST_CASE("WebSocketReader fails Close frames with an invalid code or payload")
{
    REQUIRE(readFailure(makeServerFrame(true, 0x8, "x")) == 1002);
    for (const u16 code : std::to_array<u16>({0, 999, 1004, 1005, 1006, 1015, 1016, 2999, 5000})) {
        CAPTURE(code);
        const std::string payload{static_cast<char>(code >> 8), static_cast<char>(code & 0xff)};
        REQUIRE(readFailure(makeServerFrame(true, 0x8, payload)) == 1002);
    }
    for (const u16 code : std::to_array<u16>({1000, 1001, 1002, 1003, 1007, 1011, 1014, 3000, 4999})) {
        CAPTURE(code);
        const std::string payload{static_cast<char>(code >> 8), static_cast<char>(code & 0xff)};
        REQUIRE(readFailure(makeServerFrame(true, 0x8, payload)) == 0);
    }
    REQUIRE(readFailure(makeServerFrame(true, 0x8, std::string{"\x03\xe8\xc3\x28", 4})) == 1007);
}

TEST_CASE("WebSocketReader fails text that is not valid UTF-8 with 1007")
{
    REQUIRE(readFailure(makeServerFrame(true, 0x1, "\xc3\x28")) == 1007);
    REQUIRE(readFailure(makeServerFrame(true, 0x1, "\xed\xa0\x80")) == 1007);
    REQUIRE(readFailure(makeServerFrame(true, 0x1, "\xc0\xaf")) == 1007);
    REQUIRE(readFailure(concat({makeServerFrame(false, 0x1, "\xe2\x82"), makeServerFrame(true, 0x0, "\xac")})) == 0);

    // NOTE: Binary messages may hold any bytes.
    REQUIRE(readFailure(makeServerFrame(true, 0x2, "\xc3\x28")) == 0);
}

TEST_CASE("WebSocketReader fails a message larger than the limit with 1009")
{
    REQUIRE(readFailure(makeServerFrame(true, 0x2, std::string(100, 'a')), 100) == 0);
    REQUIRE(readFailure(makeServerFrame(true, 0x2, std::string(101, 'a')), 100) == 1009);
    REQUIRE(readFailure(concat({makeServerFrame(false, 0x2, std::string(60, 'a')), makeServerFrame(true, 0x0, std::string(41, 'b'))}), 100) == 1009);

    // NOTE: The length in the header fails the message before its bytes come.
    REQUIRE(readFailure({0x82, 0x7f, 0, 0, 0, 1, 0, 0, 0, 0}, 1024) == 1009);
}

TEST_CASE("WebSocketReader returns nothing after a failure")
{
    WebSocketReader reader{0};
    reader.append(std::vector<u8>{0x83, 0x00});
    REQUIRE_FALSE(reader.next().has_value());

    reader.append(makeServerFrame(true, 0x1, "after"));
    auto result = reader.next();
    REQUIRE(result.has_value());
    REQUIRE_FALSE(result->has_value());
}
