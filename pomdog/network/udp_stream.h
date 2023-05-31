// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/signals/forward_declarations.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT UDPStream final {
private:
    std::unique_ptr<detail::NativeUDPStream> nativeStream_;

public:
    UDPStream();
    explicit UDPStream(IOService* service);

    UDPStream(const UDPStream&) = delete;
    UDPStream& operator=(const UDPStream&) = delete;

    UDPStream(UDPStream&& other);
    UDPStream& operator=(UDPStream&& other);

    ~UDPStream();

    /// Opens a UDP connection to a remote host.
    [[nodiscard]] static std::tuple<UDPStream, std::unique_ptr<Error>>
    connect(IOService* service, std::string_view address);

    /// Starts listening for incoming datagrams.
    [[nodiscard]] static std::tuple<UDPStream, std::unique_ptr<Error>>
    listen(IOService* service, std::string_view address);

    /// Closes the connection.
    void disconnect();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(const ArrayView<std::uint8_t const>& data);

    /// Writes data to address.
    [[nodiscard]] std::unique_ptr<Error>
    writeTo(const ArrayView<std::uint8_t const>& data, std::string_view address);

    /// Sets a callback function that is called when a connection is successfully established.
    [[nodiscard]] Connection
    onConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received.
    [[nodiscard]] Connection
    onRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a data packet is received from the connection.
    [[nodiscard]] Connection
    onReadFrom(std::function<void(const ArrayView<std::uint8_t>&, std::string_view address, const std::unique_ptr<Error>&)>&& callback);
};

} // namespace pomdog
