// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Network/detail/ForwardDeclarations.hpp"
#include "Pomdog/Signals/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>

namespace Pomdog {

class POMDOG_EXPORT UDPStream final {
public:
    UDPStream();
    explicit UDPStream(IOService* service);

    UDPStream(const UDPStream&) = delete;
    UDPStream& operator=(const UDPStream&) = delete;

    UDPStream(UDPStream&& other);
    UDPStream& operator=(UDPStream&& other);

    ~UDPStream();

    /// Opens a UDP connection to a remote host.
    static std::tuple<UDPStream, std::shared_ptr<Error>>
    Connect(IOService* service, const std::string_view& address);

    /// Starts listening for incoming datagrams.
    static std::tuple<UDPStream, std::shared_ptr<Error>>
    Listen(IOService* service, const std::string_view& address);

    /// Closes the connection.
    void Disconnect();

    /// Writes data to the connection.
    std::shared_ptr<Error> Write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    std::shared_ptr<Error>
    WriteTo(const ArrayView<std::uint8_t const>& data, const std::string_view& address);

    /// Sets a callback function that is called when a connection is successfully established.
    [[nodiscard]] Connection
    OnConnected(std::function<void(const std::shared_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received.
    [[nodiscard]] Connection
    OnRead(std::function<void(const ArrayView<std::uint8_t>&, const std::shared_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received from the connection.
    [[nodiscard]] Connection
    OnReadFrom(std::function<void(const ArrayView<std::uint8_t>&, const std::string_view& address, const std::shared_ptr<Error>&)>&& callback);

private:
    std::unique_ptr<Detail::NativeUDPStream> nativeStream;
};

} // namespace Pomdog
