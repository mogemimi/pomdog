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

class TCPStreamPOSIX final {
public:
    TCPStreamPOSIX() = delete;

    explicit TCPStreamPOSIX(IOService* service);

    ~TCPStreamPOSIX();

    TCPStreamPOSIX(const TCPStreamPOSIX&) = delete;
    TCPStreamPOSIX& operator=(const TCPStreamPOSIX&) = delete;

    TCPStreamPOSIX(TCPStreamPOSIX&&) = delete;
    TCPStreamPOSIX& operator=(TCPStreamPOSIX&&) = delete;

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
    [[nodiscard]] int
    getHandle() const noexcept;

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> onConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> onDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> onRead;

private:
    void readEventLoop();

private:
    std::thread blockingThread_;
    IOService* service_ = nullptr;
    int descriptor_ = -1;
    ScopedConnection eventLoopConn_;
    ScopedConnection errorConn_;
    std::optional<Duration> timeoutInterval_;
    TimePoint lastActiveTime_;
    std::atomic<bool> isConnected_ = false;
};

} // namespace pomdog::detail
