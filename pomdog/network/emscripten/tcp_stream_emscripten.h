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

class TCPStreamEmscripten final {
public:
    TCPStreamEmscripten() = delete;

    explicit TCPStreamEmscripten(IOService* service);

    ~TCPStreamEmscripten();

    TCPStreamEmscripten(const TCPStreamEmscripten&) = delete;
    TCPStreamEmscripten& operator=(const TCPStreamEmscripten&) = delete;

    TCPStreamEmscripten(TCPStreamEmscripten&&) = delete;
    TCPStreamEmscripten& operator=(TCPStreamEmscripten&&) = delete;

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

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> OnConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> OnDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> OnRead;
};

} // namespace pomdog::detail
