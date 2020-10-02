// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Network/detail/ForwardDeclarations.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
#include <WinSock2.h>

namespace Pomdog::Detail {

class UDPStreamWin32 final {
public:
    UDPStreamWin32() = delete;

    explicit UDPStreamWin32(IOService* service);

    ~UDPStreamWin32();

    UDPStreamWin32(const UDPStreamWin32&) = delete;
    UDPStreamWin32& operator=(const UDPStreamWin32&) = delete;

    UDPStreamWin32(UDPStreamWin32&&) = delete;
    UDPStreamWin32& operator=(UDPStreamWin32&&) = delete;

    /// Opens a UDP connection over UDP to a remote host.
    [[nodiscard]] std::shared_ptr<Error>
    Connect(std::string_view host, std::string_view port, const Duration& timeout);

    /// Starts listening for incoming datagrams.
    [[nodiscard]] std::shared_ptr<Error>
    Listen(std::string_view host, std::string_view port);

    /// Closes the connection.
    void Close();

    /// Writes data to the connection.
    [[nodiscard]] std::shared_ptr<Error>
    Write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    [[nodiscard]] std::shared_ptr<Error>
    WriteTo(const ArrayView<std::uint8_t const>& data, std::string_view address);

    /// Returns the native socket handle.
    [[nodiscard]] ::SOCKET GetHandle() const noexcept;

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::shared_ptr<Error>&)> OnConnected;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::shared_ptr<Error>&)> OnRead;

    /// Delegate that fires when a data packet is received from the connection.
    Delegate<void(const ArrayView<std::uint8_t>& view, std::string_view address, const std::shared_ptr<Error>&)> OnReadFrom;

private:
    void ReadEventLoop();

    void ReadFromEventLoop();

private:
    std::thread blockingThread;
    IOService* service = nullptr;
    ScopedConnection eventLoopConn;
    ScopedConnection errorConn;
    ::SOCKET descriptor = INVALID_SOCKET;
};

} // namespace Pomdog::Detail
