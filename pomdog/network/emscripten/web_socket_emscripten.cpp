// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/emscripten/web_socket_emscripten.h"
#include "pomdog/network/web_socket_check.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <emscripten.h>

#include <array>
#include <chrono>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

// NOTE: The JavaScript below is part of a C macro. It uses double quotes for
// strings and no comments, because the preprocessor reads it as C tokens, and
// clang-format must not touch it. Each socket queues events of three types:
// 1 when it opens, with the subprotocol in reason; 2 for a message; and 3 when
// it closes, with an error unless the closing handshake finished. Once a
// socket closes, its handlers are removed, so it queues nothing more.
//
// EM_JS defines variables that only the linker reads, without declarations.
POMDOG_EMCC_SUPPRESS_WARNING_PUSH
POMDOG_EMCC_SUPPRESS_WARNING("-Wmissing-variable-declarations")

// clang-format off
EM_JS_DEPS(pomdog_web_socket, "$UTF8ToString,$stringToUTF8,$lengthBytesUTF8");

EM_JS(int, pomdog_web_socket_is_supported, (), {
    return (typeof WebSocket !== "undefined") ? 1 : 0;
});

EM_JS(int, pomdog_web_socket_open, (const char* url, const char* protocols, double connectTimeout, double maxMessageSize), {
    if (!Module.pomdogWebSocket) {
        Module.pomdogWebSocket = {nextSocketID: 1, sockets: new Map()};
    }
    var state = Module.pomdogWebSocket;
    var socketID = state.nextSocketID++;
    var socket = {ws: null, queue: [], timer: 0, opened: false, closed: false, connectTimeout: connectTimeout};
    state.sockets.set(socketID, socket);

    socket.finish = function(code, reason, error) {
        if (socket.closed) {
            return;
        }
        socket.closed = true;
        clearTimeout(socket.timer);
        var ws = socket.ws;
        if (ws) {
            ws.onopen = null;
            ws.onmessage = null;
            ws.onclose = null;
            ws.onerror = null;
            if (ws.readyState < 2) {
                try {
                    ws.close();
                }
                catch (e) {
                }
            }
        }
        socket.queue.push({type: 3, text: 0, data: new Uint8Array(0), code: code, reason: reason, error: error});
    };

    var names = UTF8ToString(protocols);
    try {
        socket.ws = new WebSocket(UTF8ToString(url), (names.length > 0) ? names.split(",") : []);
    }
    catch (e) {
        socket.finish(1006, "", String((e && e.message) || e));
        return socketID;
    }
    var ws = socket.ws;
    ws.binaryType = "arraybuffer";
    socket.timer = setTimeout(function() {
        socket.finish(1006, "", "the WebSocket handshake did not finish within " + String(connectTimeout) + " ms");
    }, connectTimeout);
    ws.onopen = function() {
        clearTimeout(socket.timer);
        socket.opened = true;
        socket.queue.push({type: 1, text: 0, data: new Uint8Array(0), code: 0, reason: ws.protocol, error: ""});
    };
    ws.onmessage = function(e) {
        var isText = (typeof e.data === "string");
        var data = isText ? new TextEncoder().encode(e.data) : new Uint8Array(e.data);
        if ((maxMessageSize > 0) && (data.length > maxMessageSize)) {
            socket.finish(1006, "", "the server sent a message larger than " + String(maxMessageSize) + " bytes");
            return;
        }
        socket.queue.push({type: 2, text: isText ? 1 : 0, data: data, code: 0, reason: "", error: ""});
    };
    ws.onclose = function(e) {
        if (e.wasClean) {
            socket.finish(e.code, e.reason, "");
        }
        else if (socket.opened) {
            socket.finish(1006, "", "the connection closed without the closing handshake");
        }
        else {
            socket.finish(1006, "", "the WebSocket connection failed");
        }
    };
    return socketID;
});

EM_JS(void, pomdog_web_socket_send, (int socketID, const unsigned char* data, int size, int isText), {
    var socket = Module.pomdogWebSocket.sockets.get(socketID);
    if (!socket || socket.closed) {
        return;
    }
    var bytes = HEAPU8.slice(data, data + size);
    socket.ws.send(isText ? new TextDecoder("utf-8", {ignoreBOM: true}).decode(bytes) : bytes);
});

EM_JS(void, pomdog_web_socket_close, (int socketID, int code, const unsigned char* reason, int reasonSize), {
    var socket = Module.pomdogWebSocket.sockets.get(socketID);
    if (!socket || socket.closed) {
        return;
    }
    var text = new TextDecoder("utf-8", {ignoreBOM: true}).decode(HEAPU8.slice(reason, reason + reasonSize));
    socket.ws.close(code, text);
    clearTimeout(socket.timer);
    socket.timer = setTimeout(function() {
        socket.finish(1006, "", "the server did not answer the Close frame within " + String(socket.connectTimeout) + " ms");
    }, socket.connectTimeout);
});

EM_JS(void, pomdog_web_socket_destroy, (int socketID), {
    var state = Module.pomdogWebSocket;
    var socket = state.sockets.get(socketID);
    if (!socket) {
        return;
    }
    socket.finish(1006, "", "");
    state.sockets.delete(socketID);
});

EM_JS(int, pomdog_web_socket_peek, (int socketID, int* fields), {
    var socket = Module.pomdogWebSocket.sockets.get(socketID);
    if (!socket || (socket.queue.length === 0)) {
        return 0;
    }
    var e = socket.queue[0];
    var index = fields >> 2;
    HEAP32[index] = e.type;
    HEAP32[index + 1] = e.text;
    HEAP32[index + 2] = e.data.length;
    HEAP32[index + 3] = e.code;
    HEAP32[index + 4] = lengthBytesUTF8(e.reason);
    HEAP32[index + 5] = lengthBytesUTF8(e.error);
    return 1;
});

EM_JS(void, pomdog_web_socket_pop, (int socketID, unsigned char* data, char* reason, char* error), {
    var e = Module.pomdogWebSocket.sockets.get(socketID).queue.shift();
    var write = function(text, buffer) {
        var size = lengthBytesUTF8(text);
        if (size > 0) {
            stringToUTF8(text, buffer, size + 1);
        }
    };
    write(e.reason, reason);
    write(e.error, error);
    if (e.data.length > 0) {
        HEAPU8.set(e.data, data);
    }
});
// clang-format on

POMDOG_EMCC_SUPPRESS_WARNING_POP

namespace pomdog::network::detail {
namespace {

/// Returns the bytes of text.
[[nodiscard]] std::span<const u8>
asBytes(std::string_view text) noexcept
{
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    return std::span<const u8>{reinterpret_cast<const u8*>(text.data()), text.size()};
    POMDOG_CLANG_UNSAFE_BUFFER_END
}

/// Returns the first size bytes of buffer as text, without the null character
/// that stringToUTF8() wrote after them.
[[nodiscard]] std::string
takeText(const std::vector<char>& buffer, std::size_t size)
{
    return std::string{buffer.data(), size};
}

} // namespace

WebSocketEmscripten::WebSocketEmscripten() noexcept = default;

WebSocketEmscripten::~WebSocketEmscripten()
{
    if (socketID_ != 0) {
        pomdog_web_socket_destroy(socketID_);
    }
}

std::expected<void, std::unique_ptr<Error>>
WebSocketEmscripten::connect(std::string_view url, const WebSocketOptions& options)
{
    if (pomdog_web_socket_is_supported() == 0) {
        return std::unexpected(errors::make("this environment has no WebSocket"));
    }

    // NOTE: The subprotocols are tokens, which have no commas.
    std::string protocols;
    for (const auto& protocol : options.protocols) {
        if (!protocols.empty()) {
            protocols += ',';
        }
        protocols += protocol;
    }

    const auto connectTimeout = std::chrono::duration_cast<std::chrono::duration<f64, std::milli>>(options.connectTimeout).count();
    const std::string urlString{url};
    socketID_ = pomdog_web_socket_open(urlString.c_str(), protocols.c_str(), connectTimeout, static_cast<f64>(options.maxMessageSize));
    state_ = State::Connecting;
    return {};
}

void WebSocketEmscripten::update()
{
    std::array<int, 6> fields = {};
    while (pomdog_web_socket_peek(socketID_, fields.data()) != 0) {
        const int type = fields[0];
        const bool isText = (fields[1] != 0);
        std::vector<u8> data(static_cast<std::size_t>(fields[2]));
        const auto code = static_cast<u16>(fields[3]);
        const auto reasonSize = static_cast<std::size_t>(fields[4]);
        const auto errorSize = static_cast<std::size_t>(fields[5]);

        // NOTE: stringToUTF8() writes a null character after the text.
        std::vector<char> reason(reasonSize + 1);
        std::vector<char> error(errorSize + 1);
        pomdog_web_socket_pop(socketID_, data.data(), reason.data(), error.data());

        if (type == 1) {
            if (state_ == State::Connecting) {
                state_ = State::Open;
                protocol_ = takeText(reason, reasonSize);
                events_.push_back(WebSocketEvent{
                    .kind = StreamEventKind::Connected,
                    .messageType = WebSocketMessageType::Binary,
                    .data = {},
                    .closeCode = 0,
                    .closeReason = {},
                    .error = nullptr,
                });
            }
        }
        else if (type == 2) {
            // NOTE: After close(), messages are dropped, as on native
            // platforms.
            if (state_ == State::Open) {
                events_.push_back(WebSocketEvent{
                    .kind = StreamEventKind::Received,
                    .messageType = isText ? WebSocketMessageType::Text : WebSocketMessageType::Binary,
                    .data = std::move(data),
                    .closeCode = 0,
                    .closeReason = {},
                    .error = nullptr,
                });
            }
        }
        else if (state_ != State::Closed) {
            state_ = State::Closed;
            events_.push_back(WebSocketEvent{
                .kind = StreamEventKind::Disconnected,
                .messageType = WebSocketMessageType::Binary,
                .data = {},
                .closeCode = code,
                .closeReason = takeText(reason, reasonSize),
                .error = (errorSize > 0) ? errors::make(takeText(error, errorSize)) : nullptr,
            });
        }
    }
}

std::optional<WebSocketEvent>
WebSocketEmscripten::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
WebSocketEmscripten::sendText(std::string_view text)
{
    if (state_ != State::Open) {
        return std::unexpected(errors::make("the WebSocket is not connected"));
    }
    if (!isValidUTF8(text)) {
        return std::unexpected(errors::make("the text is not valid UTF-8"));
    }
    const auto bytes = asBytes(text);
    pomdog_web_socket_send(socketID_, bytes.data(), static_cast<int>(bytes.size()), 1);
    return {};
}

std::expected<void, std::unique_ptr<Error>>
WebSocketEmscripten::sendBinary(std::span<const u8> data)
{
    if (state_ != State::Open) {
        return std::unexpected(errors::make("the WebSocket is not connected"));
    }
    pomdog_web_socket_send(socketID_, data.data(), static_cast<int>(data.size()), 0);
    return {};
}

std::expected<void, std::unique_ptr<Error>>
WebSocketEmscripten::close(u16 code, std::string_view reason)
{
    if (auto result = checkWebSocketClose(code, reason); !result) {
        return std::unexpected(std::move(result.error()));
    }

    if (state_ == State::Connecting) {
        pomdog_web_socket_destroy(socketID_);
        socketID_ = 0;
        state_ = State::Closed;
        events_.push_back(WebSocketEvent{
            .kind = StreamEventKind::Disconnected,
            .messageType = WebSocketMessageType::Binary,
            .data = {},
            .closeCode = 1006,
            .closeReason = {},
            .error = errors::make("the WebSocket closed before it connected"),
        });
        return {};
    }
    if (state_ != State::Open) {
        return {};
    }

    const auto bytes = asBytes(reason);
    pomdog_web_socket_close(socketID_, code, bytes.data(), static_cast<int>(bytes.size()));
    state_ = State::Closing;
    return {};
}

bool WebSocketEmscripten::isConnected() const noexcept
{
    return state_ == State::Open;
}

std::string_view
WebSocketEmscripten::getProtocol() const noexcept
{
    return protocol_;
}

} // namespace pomdog::network::detail
