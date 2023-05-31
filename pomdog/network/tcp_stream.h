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

class POMDOG_EXPORT TCPStream final {
private:
    std::unique_ptr<detail::NativeTCPStream> nativeStream_;

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
    connect(IOService* service, std::string_view address);

    /// Opens a TCP connection to a remote host.
    [[nodiscard]] static std::tuple<TCPStream, std::unique_ptr<Error>>
    connect(IOService* service, std::string_view address, const Duration& timeout);

    /// Closes the connection.
    void disconnect();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool
    isConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void setTimeout(const Duration& timeout);

    /// Sets a callback function that is called when a connection is successfully established.
    [[nodiscard]] Connection
    onConnected(std::function<void(const std::unique_ptr<Error>&)>&& callback);

    /// Sets a callback function that is called when a connection is disconnected.
    [[nodiscard]] Connection
    onDisconnect(std::function<void()>&& callback);

    /// Sets a callback function that is called when a data packet is received.
    [[nodiscard]] Connection
    onRead(std::function<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)>&& callback);
};

} // namespace pomdog
