// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network {

// NOTE: An event owns its error, so it is move-only. MSVC reports the
// implicitly deleted copy operations unless these warnings are off.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
/// Reports a change of a `UDPStream`.
struct UDPStreamEvent final {
    StreamEventKind kind = StreamEventKind::Connected;

    /// The received datagram for Received. Empty for other kinds.
    std::vector<u8> data;

    /// The sender's address for Received on a listening stream, such as
    /// "127.0.0.1:30088". Empty for other kinds and for a connected stream.
    std::string address;

    /// Why the stream closed for Disconnected, such as a failed connection.
    /// nullptr for other kinds.
    std::unique_ptr<Error> error;
};
POMDOG_MSVC_SUPPRESS_WARNING_POP

/// UDPStream sends and receives datagrams.
///
/// A stream from `connectUDPStream()` exchanges datagrams with one address. A
/// stream from `listenUDPStream()` receives datagrams from any sender and
/// answers with `writeTo()`. Both report Connected first, after a later
/// `update()`, and work like `Stream` otherwise.
///
/// A stream is not thread-safe. Use it from the thread that runs the game loop.
class POMDOG_EXPORT UDPStream {
public:
    UDPStream() noexcept;

    UDPStream(const UDPStream&) = delete;
    UDPStream& operator=(const UDPStream&) = delete;

    virtual ~UDPStream();

    /// Advances the connection and queues events for what happened, such as
    /// received datagrams. Call it once per frame, before taking events.
    virtual void
    update() = 0;

    /// Takes the oldest queued event, or returns nullopt when none waits.
    [[nodiscard]] virtual std::optional<UDPStreamEvent>
    pollEvent() = 0;

    /// Sends data as one datagram to the address that the stream connected
    /// to. Fails when the stream is not connected.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) = 0;

    /// Sends data as one datagram to address, such as an address that a
    /// Received event reported.
    [[nodiscard]] virtual std::expected<void, std::unique_ptr<Error>>
    writeTo(std::span<const u8> data, std::string_view address) = 0;

    /// Closes the stream and drops its queued events. The stream reports no
    /// Disconnected event for it.
    virtual void
    close() = 0;
};

/// Starts a UDP stream that exchanges datagrams with address, such as
/// "localhost:30088".
///
/// Returns an error when the stream cannot start, and on the web, which has
/// no UDP sockets.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<UDPStream>, std::unique_ptr<Error>>
connectUDPStream(std::string_view address);

/// Starts a UDP stream that receives datagrams on address, such as
/// "localhost:30088".
///
/// Returns an error when the stream cannot bind to address, and on the web,
/// which has no UDP sockets.
[[nodiscard]] POMDOG_EXPORT std::expected<std::shared_ptr<UDPStream>, std::unique_ptr<Error>>
listenUDPStream(std::string_view address);

} // namespace pomdog::network
