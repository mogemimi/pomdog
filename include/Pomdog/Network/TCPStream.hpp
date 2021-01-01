// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Signals/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

class POMDOG_EXPORT TCPStream final {
public:
    TCPStream();
    explicit TCPStream(IOService* service);

    TCPStream(const TCPStream&) = delete;
    TCPStream& operator=(const TCPStream&) = delete;

    TCPStream(TCPStream&& other);
    TCPStream& operator=(TCPStream&& other);

    ~TCPStream();

    /// Opens a TCP connection to a remote host.
    [[nodiscard]] static std::tuple<TCPStream, std::unique_ptr<Error>>
    Connect(IOService* service, std::string_view address);

    /// Opens a TCP connection to a remote host.
    [[nodiscard]] static std::tuple<TCPStream, std::unique_ptr<Error>>
    Connect(IOService* service, std::string_view address, const Duration& timeout);

    /// Closes the connection.
    void Disconnect();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    Write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool IsConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void SetTimeout(const Duration& timeout);

    /// Sets a callback function that is called when a connection is successfully established.
    [[nodiscard]] Connection
    OnConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a connection is disconnected.
    [[nodiscard]] Connection
    OnDisconnect(std::function<void()>&& callback);

    /// Sets a callback function that is called when a data packet is received.
    [[nodiscard]] Connection
    OnRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback);

private:
    std::unique_ptr<Detail::NativeTCPStream> nativeStream;
};

} // namespace Pomdog
