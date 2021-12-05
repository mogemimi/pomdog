// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/TimePoint.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Errors.hpp"

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

namespace Pomdog::Detail {

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
    Connect(std::string_view host, std::string_view port, const Duration& timeout);

    /// Closes the connection.
    void Close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    Write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool IsConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void SetTimeout(const Duration& timeout);

    /// Returns the native socket handle.
    [[nodiscard]] int GetHandle() const noexcept;

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> OnConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> OnDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> OnRead;

private:
    void ReadEventLoop();

private:
    std::thread blockingThread;
    IOService* service = nullptr;
    int descriptor = -1;
    ScopedConnection eventLoopConn;
    ScopedConnection errorConn;
    std::optional<Duration> timeoutInterval;
    TimePoint lastActiveTime;
    std::atomic<bool> isConnected = false;
};

} // namespace Pomdog::Detail
