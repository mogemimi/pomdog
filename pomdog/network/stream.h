// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

/// Identifies what a `StreamEvent` or a `UDPStreamEvent` reports.
enum class StreamEventKind : u8 {
    /// The stream connected and can write. A TLS stream reports it after the
    /// handshake.
    Connected,

    /// The stream received bytes.
    Received,

    /// The stream closed and reports no further events.
    Disconnected,
};

// NOTE: An event owns its error, so it is move-only. MSVC reports the
// implicitly deleted copy operations unless these warnings are off.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
/// Reports a change of a `Stream`.
struct StreamEvent final {
    StreamEventKind kind = StreamEventKind::Connected;

    /// The received bytes for Received. Empty for other kinds.
    std::vector<u8> data;

    /// Why the stream closed for Disconnected, such as a failed connection or
    /// a timeout, or nullptr when the peer closed it. nullptr for other kinds.
    std::unique_ptr<Error> error;
};
POMDOG_MSVC_SUPPRESS_WARNING_POP

/// Sets how a stream connects and when it gives up.
struct StreamOptions final {
    /// Limits how long connecting to the server may take. A TLS handshake gets
    /// the same time again. Name resolution is not limited.
    Duration connectTimeout = std::chrono::seconds{5};

    /// Closes the stream when it sends and receives nothing for this long.
    /// Zero sets no limit.
    Duration idleTimeout = Duration::zero();
};

/// Stream is a connection that carries bytes in order, over TCP or TLS.
///
/// A stream connects asynchronously. Each `update()` advances the connection
/// and reads what arrived, and the resulting events wait in a queue until
/// `pollEvent()` takes them:
/// ```cpp
/// stream->update();
/// while (auto event = stream->pollEvent()) {
///     switch (event->kind) {
///     case StreamEventKind::Connected:
///         sendGreeting(*stream);
///         break;
///     case StreamEventKind::Received:
///         append(std::move(event->data));
///         break;
///     case StreamEventKind::Disconnected:
///         onClosed(event->error.get());
///         break;
///     }
/// }
/// ```
///
/// A stream is not thread-safe. Use it from the thread that runs the game loop.
class POMDOG_EXPORT Stream {
public:
    Stream() noexcept;

    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;

    virtual ~Stream();

    /// Advances the connection, sends the bytes that wait to be sent, and
    /// queues events for what happened, such as received bytes. Call it once
    /// per frame, before taking events.
    virtual void
    update() = 0;

    /// Takes the oldest queued event, or returns nullopt when none waits.
    [[nodiscard]] virtual std::optional<StreamEvent>
    pollEvent() = 0;

    /// Sends data. The bytes that the connection cannot take at once wait in
    /// the stream, and later `update()` calls send them.
    ///
    /// Fails only when the stream is not connected. A failure to send closes
    /// the stream, which then reports Disconnected with the error.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) = 0;

    /// Closes the stream, and drops its queued events and the bytes that wait
    /// to be sent. The stream reports no Disconnected event for it.
    virtual void
    close() = 0;

    /// Returns true if the stream is connected, false otherwise.
    [[nodiscard]] virtual bool
    isConnected() const noexcept = 0;
};

} // namespace pomdog::network
