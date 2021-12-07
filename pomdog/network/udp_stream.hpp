// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/network/forward_declarations.hpp"
#include "pomdog/signals/forward_declarations.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    static std::tuple<UDPStream, std::unique_ptr<Error>>
    Connect(IOService* service, std::string_view address);

    /// Starts listening for incoming datagrams.
    static std::tuple<UDPStream, std::unique_ptr<Error>>
    Listen(IOService* service, std::string_view address);

    /// Closes the connection.
    void Disconnect();

    /// Writes data to the connection.
    std::unique_ptr<Error> Write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    std::unique_ptr<Error>
    WriteTo(const ArrayView<std::uint8_t const>& data, std::string_view address);

    /// Sets a callback function that is called when a connection is successfully established.
    [[nodiscard]] Connection
    OnConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received.
    [[nodiscard]] Connection
    OnRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received from the connection.
    [[nodiscard]] Connection
    OnReadFrom(std::function<void(const ArrayView<std::uint8_t>&, std::string_view address, const std::unique_ptr<Error>&)>&& callback);

private:
    std::unique_ptr<Detail::NativeUDPStream> nativeStream;
};

} // namespace Pomdog
