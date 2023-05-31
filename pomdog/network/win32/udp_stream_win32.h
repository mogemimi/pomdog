// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/signals/delegate.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

class UDPStreamWin32 final {
private:
    std::thread blockingThread_;
    IOService* service_ = nullptr;
    ScopedConnection eventLoopConn_;
    ScopedConnection errorConn_;
    ::SOCKET descriptor_ = INVALID_SOCKET;

public:
    UDPStreamWin32() = delete;

    explicit UDPStreamWin32(IOService* service);

    ~UDPStreamWin32();

    UDPStreamWin32(const UDPStreamWin32&) = delete;
    UDPStreamWin32& operator=(const UDPStreamWin32&) = delete;

    UDPStreamWin32(UDPStreamWin32&&) = delete;
    UDPStreamWin32& operator=(UDPStreamWin32&&) = delete;

    /// Opens a UDP connection over UDP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    connect(std::string_view host, std::string_view port, const Duration& timeout);

    /// Starts listening for incoming datagrams.
    [[nodiscard]] std::unique_ptr<Error>
    listen(std::string_view host, std::string_view port);

    /// Closes the connection.
    void close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    [[nodiscard]] std::unique_ptr<Error>
    writeTo(const ArrayView<std::uint8_t const>& data, std::string_view address);

    /// Returns the native socket handle.
    [[nodiscard]] ::SOCKET
    getHandle() const noexcept;

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> onConnected;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> onRead;

    /// Delegate that fires when a data packet is received from the connection.
    Delegate<void(const ArrayView<std::uint8_t>& view, std::string_view address, const std::unique_ptr<Error>&)> onReadFrom;

private:
    void readEventLoop();

    void readFromEventLoop();
};

} // namespace pomdog::detail
