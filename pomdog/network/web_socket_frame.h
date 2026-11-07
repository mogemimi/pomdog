// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstddef>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// Identifies the kind of a WebSocket frame (RFC 6455, section 5.2).
enum class WebSocketOpcode : u8 {
    Continuation = 0x0,
    Text = 0x1,
    Binary = 0x2,
    Close = 0x8,
    Ping = 0x9,
    Pong = 0xA,
};

/// WebSocketMaskKey is the masking key of a frame that a client sends.
using WebSocketMaskKey = std::array<u8, 4>;

/// Returns a frame that a client sends, with FIN set and payload masked by
/// maskKey (RFC 6455, section 5.3).
[[nodiscard]] std::vector<u8>
encodeWebSocketFrame(WebSocketOpcode opcode, std::span<const u8> payload, const WebSocketMaskKey& maskKey);

/// Returns the payload of a Close frame with code and reason, or an empty
/// payload when code is 1005, which means that a frame holds no code.
[[nodiscard]] std::vector<u8>
makeWebSocketClosePayload(u16 code, std::string_view reason);

/// WebSocketMessage is a data message that `WebSocketReader` put together from
/// its frames, or a control frame.
struct WebSocketMessage final {
    /// Text or Binary for a data message, or Close, Ping, or Pong.
    WebSocketOpcode opcode = WebSocketOpcode::Binary;

    /// The data of a message, the payload of a Ping or Pong frame, or the
    /// reason of a Close frame.
    std::vector<u8> payload;

    /// The status code of a Close frame, or 1005 when the frame holds none.
    /// Zero for other opcodes.
    u16 closeCode = 0;
};

// NOTE: The error owns an Error, so it is move-only. MSVC reports the
// implicitly deleted copy operations unless these warnings are off.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
/// WebSocketReadError tells why the frames of a server break the protocol,
/// and the status code that the client sends in its Close frame.
struct WebSocketReadError final {
    /// 1002 for a protocol error, 1007 for text that is not valid UTF-8, or
    /// 1009 for a message that is too large (RFC 6455, section 7.4.1).
    u16 closeCode = 1002;

    std::unique_ptr<Error> error;
};
POMDOG_MSVC_SUPPRESS_WARNING_POP

/// WebSocketReader reads the messages and control frames that a server sends
/// from the bytes of a connection (RFC 6455, section 5).
///
/// It puts fragmented messages together, and returns control frames as they
/// come, also between the fragments of a message. A frame that breaks the
/// protocol fails, such as a masked frame, a reserved bit, an unknown opcode,
/// or a Close frame with an invalid code, and so does text that is not valid
/// UTF-8. After a failure, the reader returns nothing more.
class WebSocketReader final {
public:
    /// Creates a reader that fails a message longer than maxMessageSize bytes,
    /// once the length of a frame tells it. Zero sets no limit.
    explicit WebSocketReader(std::size_t maxMessageSize) noexcept;

    WebSocketReader(const WebSocketReader&) = delete;
    WebSocketReader& operator=(const WebSocketReader&) = delete;
    WebSocketReader(WebSocketReader&&) = delete;
    WebSocketReader& operator=(WebSocketReader&&) = delete;

    /// Adds the next bytes of the connection.
    void append(std::span<const u8> data);

    /// Takes the next message or control frame, or returns nullopt when it
    /// needs more bytes.
    [[nodiscard]] std::expected<std::optional<WebSocketMessage>, WebSocketReadError>
    next();

private:
    /// Returns the bytes that `next()` has not read.
    [[nodiscard]] std::span<const u8>
    unread() const noexcept;

    /// Drops the read bytes once they are at least half of the buffer.
    void compact();

    std::vector<u8> buffer_;

    /// The fragments of the message so far.
    std::vector<u8> message_;

    std::size_t offset_ = 0;
    std::size_t maxMessageSize_ = 0;

    /// Text or Binary for the message in fragments.
    WebSocketOpcode messageOpcode_ = WebSocketOpcode::Binary;

    /// True while a message waits for its final fragment.
    bool hasMessage_ = false;

    bool failed_ = false;
};

} // namespace pomdog::network::detail
