// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/web_socket_frame.h"
#include "pomdog/network/web_socket_check.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <iterator>
#include <limits>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// The longest payload of a control frame (RFC 6455, section 5.5).
constexpr u64 maxControlPayloadSize = 125;

/// Returns bytes as text.
[[nodiscard]] std::string_view
asText(std::span<const u8> bytes) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::string_view{reinterpret_cast<const char*>(bytes.data()), bytes.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns the bytes of text.
[[nodiscard]] std::span<const u8>
asBytes(std::string_view text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns true if code may appear in a Close frame (RFC 6455, section 7.4,
/// and the IANA registry of status codes).
[[nodiscard]] bool
isValidCloseCode(u16 code) noexcept
{
    // NOTE: 1004 is reserved, and 1005, 1006, and 1015 tell the application
    // what happened without a code, so a frame must not hold them.
    if ((code >= 1000) && (code <= 1003)) {
        return true;
    }
    if ((code >= 1007) && (code <= 1014)) {
        return true;
    }
    return (code >= 3000) && (code <= 4999);
}

[[nodiscard]] WebSocketReadError
makeReadError(u16 closeCode, std::string&& message)
{
    return WebSocketReadError{.closeCode = closeCode, .error = errors::make(std::move(message))};
}

} // namespace

std::vector<u8>
encodeWebSocketFrame(WebSocketOpcode opcode, std::span<const u8> payload, const WebSocketMaskKey& maskKey)
{
    std::vector<u8> frame;
    frame.reserve(14 + payload.size());
    frame.push_back(static_cast<u8>(0x80 | static_cast<u8>(opcode)));

    // NOTE: A client masks every frame, so the MASK bit is always set.
    constexpr u8 maskBit = 0x80;
    const auto size = static_cast<u64>(payload.size());
    if (size < 126) {
        frame.push_back(static_cast<u8>(maskBit | size));
    }
    else if (size <= 0xffff) {
        frame.push_back(maskBit | 126);
        frame.push_back(static_cast<u8>(size >> 8));
        frame.push_back(static_cast<u8>(size & 0xff));
    }
    else {
        frame.push_back(maskBit | 127);
        for (int shift = 56; shift >= 0; shift -= 8) {
            frame.push_back(static_cast<u8>((size >> shift) & 0xff));
        }
    }

    frame.insert(std::end(frame), std::begin(maskKey), std::end(maskKey));
    for (std::size_t i = 0; i < payload.size(); ++i) {
        frame.push_back(static_cast<u8>(payload[i] ^ maskKey[i % maskKey.size()]));
    }
    return frame;
}

std::vector<u8>
makeWebSocketClosePayload(u16 code, std::string_view reason)
{
    std::vector<u8> payload;
    if (code == 1005) {
        return payload;
    }
    payload.reserve(2 + reason.size());
    payload.push_back(static_cast<u8>(code >> 8));
    payload.push_back(static_cast<u8>(code & 0xff));
    const auto bytes = asBytes(reason);
    payload.insert(std::end(payload), std::begin(bytes), std::end(bytes));
    return payload;
}

WebSocketReader::WebSocketReader(std::size_t maxMessageSize) noexcept
    : maxMessageSize_(maxMessageSize)
{
}

void WebSocketReader::append(std::span<const u8> data)
{
    if (failed_) {
        return;
    }
    buffer_.insert(std::end(buffer_), std::begin(data), std::end(data));
}

std::expected<std::optional<WebSocketMessage>, WebSocketReadError>
WebSocketReader::next()
{
    if (failed_) {
        return std::nullopt;
    }
    const auto fail = [this](u16 closeCode, std::string&& message) {
        failed_ = true;
        return std::unexpected(makeReadError(closeCode, std::move(message)));
    };

    for (;;) {
        const auto data = unread();
        if (data.size() < 2) {
            return std::nullopt;
        }

        const bool fin = (data[0] & 0x80) != 0;
        const auto opcodeValue = static_cast<u8>(data[0] & 0x0f);
        if ((data[0] & 0x70) != 0) {
            return fail(1002, "the server sent a frame with a reserved bit, though no extension was negotiated");
        }
        if ((data[1] & 0x80) != 0) {
            return fail(1002, "the server sent a masked frame");
        }

        u64 length = data[1] & 0x7f;
        std::size_t headerSize = 2;
        if (length == 126) {
            if (data.size() < 4) {
                return std::nullopt;
            }
            length = (static_cast<u64>(data[2]) << 8) | data[3];
            headerSize = 4;
        }
        else if (length == 127) {
            if (data.size() < 10) {
                return std::nullopt;
            }
            length = 0;
            for (std::size_t i = 2; i < 10; ++i) {
                length = (length << 8) | data[i];
            }
            if ((length >> 63) != 0) {
                return fail(1002, "the server sent a frame whose length has the most significant bit");
            }
            headerSize = 10;
        }

        const auto opcode = static_cast<WebSocketOpcode>(opcodeValue);
        const bool isControl = (opcodeValue & 0x08) != 0;
        if (isControl) {
            if ((opcode != WebSocketOpcode::Close) && (opcode != WebSocketOpcode::Ping) && (opcode != WebSocketOpcode::Pong)) {
                return fail(1002, "the server sent a frame with the unknown opcode " + std::to_string(opcodeValue));
            }
            if (!fin) {
                return fail(1002, "the server sent a fragmented control frame");
            }
            if (length > maxControlPayloadSize) {
                return fail(1002, "the server sent a control frame longer than 125 bytes");
            }
        }
        else {
            if ((opcode != WebSocketOpcode::Continuation) && (opcode != WebSocketOpcode::Text) && (opcode != WebSocketOpcode::Binary)) {
                return fail(1002, "the server sent a frame with the unknown opcode " + std::to_string(opcodeValue));
            }
            if ((opcode == WebSocketOpcode::Continuation) && !hasMessage_) {
                return fail(1002, "the server sent a continuation frame without a message to continue");
            }
            if ((opcode != WebSocketOpcode::Continuation) && hasMessage_) {
                return fail(1002, "the server started a message before the previous one ended");
            }

            // NOTE: Fail as soon as the length tells that the message is too
            // large, rather than after the bytes arrive.
            const u64 messageSize = ((opcode == WebSocketOpcode::Continuation) ? message_.size() : 0) + length;
            const u64 limit = (maxMessageSize_ > 0) ? maxMessageSize_ : std::numeric_limits<std::size_t>::max();
            if ((length > limit) || (messageSize > limit)) {
                return fail(1009, "the server sent a message larger than " + std::to_string(limit) + " bytes");
            }
        }

        if ((data.size() - headerSize) < length) {
            return std::nullopt;
        }
        const auto payload = data.subspan(headerSize, static_cast<std::size_t>(length));
        offset_ += headerSize + payload.size();

        if (isControl) {
            WebSocketMessage message;
            message.opcode = opcode;
            if (opcode == WebSocketOpcode::Close) {
                if (payload.size() == 1) {
                    return fail(1002, "the server sent a Close frame with a 1-byte payload");
                }
                message.closeCode = 1005;
                if (payload.size() >= 2) {
                    message.closeCode = static_cast<u16>((payload[0] << 8) | payload[1]);
                    if (!isValidCloseCode(message.closeCode)) {
                        return fail(1002, "the server sent a Close frame with the invalid code " + std::to_string(message.closeCode));
                    }
                    const auto reason = payload.subspan(2);
                    if (!isValidUTF8(asText(reason))) {
                        return fail(1007, "the server sent a close reason that is not valid UTF-8");
                    }
                    message.payload.assign(std::begin(reason), std::end(reason));
                }
            }
            else {
                message.payload.assign(std::begin(payload), std::end(payload));
            }
            compact();
            return message;
        }

        if (opcode != WebSocketOpcode::Continuation) {
            messageOpcode_ = opcode;
            hasMessage_ = true;
        }
        message_.insert(std::end(message_), std::begin(payload), std::end(payload));
        compact();
        if (!fin) {
            continue;
        }

        hasMessage_ = false;
        WebSocketMessage message;
        message.opcode = messageOpcode_;
        message.payload = std::exchange(message_, {});
        if ((message.opcode == WebSocketOpcode::Text) && !isValidUTF8(asText(message.payload))) {
            return fail(1007, "the server sent text that is not valid UTF-8");
        }
        return message;
    }
}

std::span<const u8>
WebSocketReader::unread() const noexcept
{
    return std::span<const u8>{buffer_}.subspan(offset_);
}

void WebSocketReader::compact()
{
    if ((offset_ * 2) >= buffer_.size()) {
        buffer_.erase(std::begin(buffer_), std::next(std::begin(buffer_), static_cast<std::ptrdiff_t>(offset_)));
        offset_ = 0;
    }
}

} // namespace pomdog::network::detail
