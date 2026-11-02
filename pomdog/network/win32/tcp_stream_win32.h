// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/stream.h"
#include "pomdog/network/win32/winsock_win32.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <chrono>
#include <deque>
#include <expected>
#include <future>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

/// TCPStreamWin32 is a TCP stream built on Winsock.
///
/// Only the blocking socket connection runs on a worker thread; `update()`
/// takes its result.
class TCPStreamWin32 final : public Stream {
public:
    TCPStreamWin32() noexcept;

    ~TCPStreamWin32() override;

    /// Starts connecting to host on port.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(std::string_view host, std::string_view port, const StreamOptions& options);

    void update() override;

    [[nodiscard]] std::optional<StreamEvent>
    pollEvent() override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) override;

    void close() override;

    [[nodiscard]] bool
    isConnected() const noexcept override;

private:
    /// Closes the socket and queues a Disconnected event with err.
    void disconnect(std::unique_ptr<Error>&& err);

    void closeSocket() noexcept;

    void readSocket();

private:
    WinsockLibrary winsock_;
    std::future<std::expected<::SOCKET, std::unique_ptr<Error>>> connectResult_;
    std::deque<StreamEvent> events_;
    ::SOCKET descriptor_ = INVALID_SOCKET;
    Duration idleTimeout_ = Duration::zero();
    std::chrono::steady_clock::time_point lastActiveTime_;
    bool isClosed_ = false;
};

} // namespace pomdog::network::detail
