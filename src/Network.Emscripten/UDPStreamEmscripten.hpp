// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <thread>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

class UDPStreamEmscripten final {
public:
    UDPStreamEmscripten() = delete;

    explicit UDPStreamEmscripten(IOService* service);

    ~UDPStreamEmscripten();

    UDPStreamEmscripten(const UDPStreamEmscripten&) = delete;
    UDPStreamEmscripten& operator=(const UDPStreamEmscripten&) = delete;

    UDPStreamEmscripten(UDPStreamEmscripten&&) = delete;
    UDPStreamEmscripten& operator=(UDPStreamEmscripten&&) = delete;

    /// Opens a UDP connection over UDP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    Connect(std::string_view host, std::string_view port, const Duration& timeout);

    /// Starts listening for incoming datagrams.
    [[nodiscard]] std::unique_ptr<Error>
    Listen(std::string_view host, std::string_view port);

    /// Closes the connection.
    void Close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    Write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    [[nodiscard]] std::unique_ptr<Error>
    WriteTo(const ArrayView<std::uint8_t const>& data, std::string_view address);

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> OnConnected;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> OnRead;

    /// Delegate that fires when a data packet is received from the connection.
    Delegate<void(const ArrayView<std::uint8_t>& view, std::string_view address, const std::unique_ptr<Error>&)> OnReadFrom;
};

} // namespace Pomdog::Detail
