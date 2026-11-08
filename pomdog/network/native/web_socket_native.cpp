// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/native/web_socket_native.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/http_parser.h"
#include "pomdog/network/http_request_check.h"
#include "pomdog/network/http_response.h"
#include "pomdog/network/mbedtls/mbedtls_helper.h"
#include "pomdog/network/tcp_stream.h"
#include "pomdog/network/tls_stream.h"
#include "pomdog/network/web_socket_check.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <iterator>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

/// The GUID that the server appends to the key of the client to compute
/// Sec-WebSocket-Accept (RFC 6455, section 1.3).
constexpr std::string_view webSocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

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

[[nodiscard]] char
toLower(char c) noexcept
{
    if ((c >= 'A') && (c <= 'Z')) {
        return static_cast<char>((c - 'A') + 'a');
    }
    return c;
}

[[nodiscard]] bool
equalsIgnoringCase(std::string_view a, std::string_view b) noexcept
{
    return std::ranges::equal(a, b, [](char x, char y) { return toLower(x) == toLower(y); });
}

/// Returns true if value, a comma-separated list, has token, ignoring case.
[[nodiscard]] bool
hasToken(std::string_view value, std::string_view token) noexcept
{
    while (true) {
        const auto comma = value.find(',');
        auto element = value.substr(0, comma);
        while (!element.empty() && ((element.front() == ' ') || (element.front() == '\t'))) {
            element.remove_prefix(1);
        }
        while (!element.empty() && ((element.back() == ' ') || (element.back() == '\t'))) {
            element.remove_suffix(1);
        }
        if (equalsIgnoringCase(element, token)) {
            return true;
        }
        if (comma == std::string_view::npos) {
            return false;
        }
        value.remove_prefix(comma + 1);
    }
}

/// Returns the value of the first header named name, compared without case,
/// or nullopt when the response has none.
[[nodiscard]] std::optional<std::string_view>
findHeader(const HTTPResponse& response, std::string_view name) noexcept
{
    for (const auto& [key, value] : response.headers) {
        if (equalsIgnoringCase(key, name)) {
            return value;
        }
    }
    return std::nullopt;
}

/// Returns data in Base64 with padding (RFC 4648, section 4).
[[nodiscard]] std::string
encodeBase64(std::span<const u8> data)
{
    constexpr std::string_view alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    result.reserve(((data.size() + 2) / 3) * 4);
    for (std::size_t i = 0; i < data.size(); i += 3) {
        const auto remaining = data.size() - i;
        u32 group = static_cast<u32>(data[i]) << 16;
        if (remaining > 1) {
            group |= static_cast<u32>(data[i + 1]) << 8;
        }
        if (remaining > 2) {
            group |= static_cast<u32>(data[i + 2]);
        }
        result += alphabet[(group >> 18) & 0x3f];
        result += alphabet[(group >> 12) & 0x3f];
        result += (remaining > 1) ? alphabet[(group >> 6) & 0x3f] : '=';
        result += (remaining > 2) ? alphabet[group & 0x3f] : '=';
    }
    return result;
}

[[nodiscard]] std::string
formatMilliseconds(Duration duration)
{
    return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) + " ms";
}

/// Checks the handshake response of the server (RFC 6455, section 4.1), and
/// returns the subprotocol that it picked, or an empty string for none.
[[nodiscard]] std::expected<std::string, std::unique_ptr<Error>>
checkHandshakeResponse(
    const HTTPResponse& response,
    std::string_view expectedAccept,
    std::span<const std::string> protocols)
{
    // NOTE: Browsers follow no redirect for a WebSocket, so neither does the
    // client.
    if (response.statusCode != 101) {
        return std::unexpected(errors::make("the server answered the WebSocket handshake with status " + response.status));
    }
    if (const auto upgrade = findHeader(response, "Upgrade"); !upgrade || !equalsIgnoringCase(*upgrade, "websocket")) {
        return std::unexpected(errors::make("the WebSocket handshake response has no Upgrade: websocket header"));
    }
    if (const auto connection = findHeader(response, "Connection"); !connection || !hasToken(*connection, "Upgrade")) {
        return std::unexpected(errors::make("the WebSocket handshake response has no Connection: Upgrade header"));
    }
    if (const auto accept = findHeader(response, "Sec-WebSocket-Accept"); !accept || (*accept != expectedAccept)) {
        return std::unexpected(errors::make("the WebSocket handshake response has a wrong Sec-WebSocket-Accept header"));
    }
    if (const auto extensions = findHeader(response, "Sec-WebSocket-Extensions"); extensions && !extensions->empty()) {
        return std::unexpected(errors::make("the server answered the WebSocket handshake with the extensions " + std::string{*extensions} + ", which the client did not offer"));
    }

    // NOTE: RFC 6455 lets a server pick none, but browsers fail the connection
    // when the client offered some.
    const auto protocol = findHeader(response, "Sec-WebSocket-Protocol");
    if (!protocol || protocol->empty()) {
        if (!protocols.empty()) {
            return std::unexpected(errors::make("the server picked none of the WebSocket subprotocols that the client offered"));
        }
        return std::string{};
    }
    if (std::ranges::find(protocols, *protocol) == std::end(protocols)) {
        return std::unexpected(errors::make("the server picked the WebSocket subprotocol " + std::string{*protocol} + ", which the client did not offer"));
    }
    return std::string{*protocol};
}

} // namespace

WebSocketNative::WebSocketNative() noexcept = default;

WebSocketNative::~WebSocketNative()
{
    if (stream_ != nullptr) {
        stream_->close();
    }
}

std::expected<void, std::unique_ptr<Error>>
WebSocketNative::connect(const RequestURL& url, const WebSocketOptions& options)
{
    if (auto result = initializePSACrypto(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    std::array<u8, 16> nonce = {};
    if (auto result = generateRandomBytes(nonce); !result) {
        return std::unexpected(std::move(result.error()));
    }
    const auto key = encodeBase64(nonce);
    auto hash = computeSHA1(asBytes(key + std::string{webSocketGUID}));
    if (!hash) {
        return std::unexpected(errors::wrap(std::move(hash.error()), "failed to compute Sec-WebSocket-Accept"));
    }
    expectedAccept_ = encodeBase64(*hash);

    protocols_ = options.protocols;
    connectTimeout_ = options.connectTimeout;
    maxMessageSize_ = options.maxMessageSize;

    // NOTE: The request has only the headers of the protocol, since browsers
    // let a page add none, and so the client offers no extensions.
    handshakeRequest_ = "GET " + url.resource + " HTTP/1.1\r\n";
    handshakeRequest_ += "Host: " + url.host + "\r\n";
    handshakeRequest_ += "Upgrade: websocket\r\n";
    handshakeRequest_ += "Connection: Upgrade\r\n";
    handshakeRequest_ += "Sec-WebSocket-Key: " + key + "\r\n";
    handshakeRequest_ += "Sec-WebSocket-Version: 13\r\n";
    if (!protocols_.empty()) {
        handshakeRequest_ += "Sec-WebSocket-Protocol: ";
        for (std::size_t i = 0; i < protocols_.size(); ++i) {
            if (i > 0) {
                handshakeRequest_ += ", ";
            }
            handshakeRequest_ += protocols_[i];
        }
        handshakeRequest_ += "\r\n";
    }
    handshakeRequest_ += "\r\n";

    StreamOptions streamOptions;
    streamOptions.connectTimeout = connectTimeout_;
    streamOptions.idleTimeout = Duration::zero();

    auto stream = url.secure
                      ? connectTLSStream(url.address, options.trustStore, streamOptions)
                      : connectTCPStream(url.address, streamOptions);
    if (!stream) {
        return std::unexpected(std::move(stream.error()));
    }
    stream_ = std::move(*stream);
    deadline_ = makeDeadline();
    state_ = State::Connecting;
    return {};
}

void WebSocketNative::update()
{
    if (state_ == State::Closed) {
        return;
    }

    stream_->update();
    while (state_ != State::Closed) {
        auto event = stream_->pollEvent();
        if (!event) {
            break;
        }
        handleStreamEvent(std::move(*event));
    }

    if ((state_ == State::Closed) || (state_ == State::Open)) {
        return;
    }
    if (std::chrono::steady_clock::now() < deadline_) {
        return;
    }
    if (state_ == State::Closing) {
        // NOTE: After the closing handshake, the server closes the
        // connection first (RFC 6455, section 7.1.1). One that does not gets
        // closed here.
        if (isCloseReceived_) {
            finish();
        }
        else {
            fail(std::nullopt, errors::make("the server did not answer the Close frame within " + formatMilliseconds(connectTimeout_)));
        }
        return;
    }
    fail(std::nullopt, errors::make("the WebSocket handshake did not finish within " + formatMilliseconds(connectTimeout_)));
}

std::optional<WebSocketEvent>
WebSocketNative::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
WebSocketNative::sendText(std::string_view text)
{
    if (state_ != State::Open) {
        return std::unexpected(errors::make("the WebSocket is not connected"));
    }
    if (!isValidUTF8(text)) {
        return std::unexpected(errors::make("the text is not valid UTF-8"));
    }
    return sendFrame(WebSocketOpcode::Text, asBytes(text));
}

std::expected<void, std::unique_ptr<Error>>
WebSocketNative::sendBinary(std::span<const u8> data)
{
    if (state_ != State::Open) {
        return std::unexpected(errors::make("the WebSocket is not connected"));
    }
    return sendFrame(WebSocketOpcode::Binary, data);
}

std::expected<void, std::unique_ptr<Error>>
WebSocketNative::close(u16 code, std::string_view reason)
{
    if (auto result = checkWebSocketClose(code, reason); !result) {
        return std::unexpected(std::move(result.error()));
    }

    if ((state_ == State::Connecting) || (state_ == State::Handshaking)) {
        fail(std::nullopt, errors::make("the WebSocket closed before it connected"));
        return {};
    }
    if (state_ != State::Open) {
        return {};
    }

    // NOTE: A failure to send means that the stream closed, and it reports
    // Disconnected in the next update().
    const auto payload = makeWebSocketClosePayload(code, reason);
    [[maybe_unused]] const auto sent = sendFrame(WebSocketOpcode::Close, payload);
    isCloseSent_ = true;
    state_ = State::Closing;
    deadline_ = makeDeadline();
    return {};
}

bool WebSocketNative::isConnected() const noexcept
{
    return state_ == State::Open;
}

std::string_view
WebSocketNative::getProtocol() const noexcept
{
    return protocol_;
}

void WebSocketNative::handleStreamEvent(StreamEvent&& event)
{
    if (event.kind == StreamEventKind::Connected) {
        if (state_ != State::Connecting) {
            return;
        }
        if (auto result = stream_->write(asBytes(handshakeRequest_)); !result) {
            fail(std::nullopt, std::move(result.error()));
            return;
        }
        handshakeRequest_ = {};
        state_ = State::Handshaking;
        return;
    }

    if (event.kind == StreamEventKind::Received) {
        if (state_ == State::Handshaking) {
            readHandshakeResponse(event.data);
        }
        else if (((state_ == State::Open) || (state_ == State::Closing)) && !isCloseReceived_) {
            reader_->append(event.data);
            readFrames();
        }
        return;
    }

    POMDOG_ASSERT(event.kind == StreamEventKind::Disconnected);
    if ((state_ == State::Closing) && isCloseReceived_) {
        finish();
        return;
    }

    auto err = std::move(event.error);
    if (err == nullptr) {
        if ((state_ == State::Connecting) || (state_ == State::Handshaking)) {
            err = errors::make("the server closed the connection during the WebSocket handshake");
        }
        else if (state_ == State::Open) {
            err = errors::make("the server closed the connection without a Close frame");
        }
        else {
            err = errors::make("the server closed the connection before it answered the Close frame");
        }
    }
    fail(std::nullopt, std::move(err));
}

void WebSocketNative::readHandshakeResponse(std::span<const u8> data)
{
    handshakeBuffer_.insert(std::end(handshakeBuffer_), std::begin(data), std::end(data));

    const auto text = asText(handshakeBuffer_);
    const auto end = text.find("\r\n\r\n");
    const auto headerSize = (end == std::string_view::npos) ? text.size() : (end + 4);
    if (headerSize > maxResponseHeaderSize) {
        fail(std::nullopt, errors::make("the WebSocket handshake response is larger than " + std::to_string(maxResponseHeaderSize) + " bytes"));
        return;
    }
    if (end == std::string_view::npos) {
        return;
    }

    HTTPResponse response;
    if (auto result = parseResponseHeader(response, text.substr(0, headerSize)); !result) {
        fail(std::nullopt, errors::wrap(std::move(result.error()), "the WebSocket handshake response is invalid"));
        return;
    }
    auto protocol = checkHandshakeResponse(response, expectedAccept_, protocols_);
    if (!protocol) {
        fail(std::nullopt, std::move(protocol.error()));
        return;
    }

    protocol_ = std::move(*protocol);
    state_ = State::Open;
    events_.push_back(WebSocketEvent{
        .kind = StreamEventKind::Connected,
        .messageType = WebSocketMessageType::Binary,
        .data = {},
        .closeCode = 0,
        .closeReason = {},
        .error = nullptr,
    });

    // NOTE: The frames that came with the response follow it.
    reader_.emplace(maxMessageSize_);
    reader_->append(std::span<const u8>{handshakeBuffer_}.subspan(headerSize));
    handshakeBuffer_ = {};
    readFrames();
}

void WebSocketNative::readFrames()
{
    while ((state_ == State::Open) || ((state_ == State::Closing) && !isCloseReceived_)) {
        auto next = reader_->next();
        if (!next) {
            fail(next.error().closeCode, std::move(next.error().error));
            return;
        }
        if (!next->has_value()) {
            return;
        }
        auto message = std::move(**next);

        if ((message.opcode == WebSocketOpcode::Text) || (message.opcode == WebSocketOpcode::Binary)) {
            // NOTE: Browsers decode a text message without its byte order mark
            // (UTF-8 decode of the Encoding standard), so the client drops it
            // too.
            constexpr std::string_view byteOrderMark = "\xef\xbb\xbf";
            if ((message.opcode == WebSocketOpcode::Text) && asText(message.payload).starts_with(byteOrderMark)) {
                message.payload.erase(std::begin(message.payload), std::next(std::begin(message.payload), static_cast<std::ptrdiff_t>(byteOrderMark.size())));
            }

            // NOTE: After close(), messages are dropped, as browsers drop them.
            if (state_ == State::Open) {
                events_.push_back(WebSocketEvent{
                    .kind = StreamEventKind::Received,
                    .messageType = (message.opcode == WebSocketOpcode::Text) ? WebSocketMessageType::Text : WebSocketMessageType::Binary,
                    .data = std::move(message.payload),
                    .closeCode = 0,
                    .closeReason = {},
                    .error = nullptr,
                });
            }
        }
        else if (message.opcode == WebSocketOpcode::Ping) {
            if (!isCloseSent_) {
                [[maybe_unused]] const auto sent = sendFrame(WebSocketOpcode::Pong, message.payload);
            }
        }
        else if (message.opcode == WebSocketOpcode::Close) {
            isCloseReceived_ = true;
            closeCode_ = message.closeCode;
            closeReason_.assign(asText(message.payload));
            if (!isCloseSent_) {
                // NOTE: Answer with the code that the server sent, as RFC
                // 6455, section 5.5.1, suggests.
                const auto payload = makeWebSocketClosePayload(message.closeCode, {});
                [[maybe_unused]] const auto sent = sendFrame(WebSocketOpcode::Close, payload);
                isCloseSent_ = true;
            }
            state_ = State::Closing;
            deadline_ = makeDeadline();
            return;
        }
    }
}

std::expected<void, std::unique_ptr<Error>>
WebSocketNative::sendFrame(WebSocketOpcode opcode, std::span<const u8> payload)
{
    // NOTE: A client masks each frame with a new key that the server cannot
    // predict (RFC 6455, section 5.3).
    WebSocketMaskKey maskKey = {};
    if (auto result = generateRandomBytes(maskKey); !result) {
        return std::unexpected(std::move(result.error()));
    }
    const auto frame = encodeWebSocketFrame(opcode, payload, maskKey);
    return stream_->write(frame);
}

void WebSocketNative::fail(std::optional<u16> closeCode, std::unique_ptr<Error>&& err)
{
    if (closeCode && (state_ == State::Open) && !isCloseSent_) {
        // NOTE: Tell the server why the connection fails (RFC 6455, section
        // 7.1.7). The stream closes right after, so the frame goes out only
        // when the connection takes it at once.
        const auto payload = makeWebSocketClosePayload(*closeCode, {});
        [[maybe_unused]] const auto sent = sendFrame(WebSocketOpcode::Close, payload);
        isCloseSent_ = true;
    }
    stream_->close();
    state_ = State::Closed;
    events_.push_back(WebSocketEvent{
        .kind = StreamEventKind::Disconnected,
        .messageType = WebSocketMessageType::Binary,
        .data = {},
        .closeCode = 1006,
        .closeReason = {},
        .error = std::move(err),
    });
}

void WebSocketNative::finish()
{
    stream_->close();
    state_ = State::Closed;
    events_.push_back(WebSocketEvent{
        .kind = StreamEventKind::Disconnected,
        .messageType = WebSocketMessageType::Binary,
        .data = {},
        .closeCode = closeCode_,
        .closeReason = std::move(closeReason_),
        .error = nullptr,
    });
}

std::chrono::steady_clock::time_point
WebSocketNative::makeDeadline() const noexcept
{
    return std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(connectTimeout_);
}

} // namespace pomdog::network::detail
