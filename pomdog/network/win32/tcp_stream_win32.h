// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/signals/delegate.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class TCPStreamWin32 final {
private:
    std::thread blockingThread_;
    IOService* service_ = nullptr;
    ::SOCKET descriptor_ = INVALID_SOCKET;
    ScopedConnection eventLoopConn_;
    ScopedConnection errorConn_;
    std::optional<Duration> timeoutInterval_;
    TimePoint lastActiveTime_;
    std::atomic<bool> isConnected_ = false;

public:
    TCPStreamWin32() = delete;

    explicit TCPStreamWin32(IOService* service);

    ~TCPStreamWin32();

    TCPStreamWin32(const TCPStreamWin32&) = delete;
    TCPStreamWin32& operator=(const TCPStreamWin32&) = delete;

    TCPStreamWin32(TCPStreamWin32&&) = delete;
    TCPStreamWin32& operator=(TCPStreamWin32&&) = delete;

    /// Opens a TCP connection over TCP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    connect(std::string_view host, std::string_view port, const Duration& timeout);

    /// Closes the connection.
    void close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool
    isConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void setTimeout(const Duration& timeout);

    /// Returns the native socket handle.
    [[nodiscard]] ::SOCKET
    getHandle() const noexcept;

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> onConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> onDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> onRead;

private:
    void readEventLoop();
};

} // namespace pomdog::detail
