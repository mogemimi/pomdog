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

class POMDOG_EXPORT TLSStream final {
public:
    TLSStream();
    explicit TLSStream(IOService* service);

    TLSStream(const TLSStream&) = delete;
    TLSStream& operator=(const TLSStream&) = delete;

    TLSStream(TLSStream&& other);
    TLSStream& operator=(TLSStream&& other);

    ~TLSStream();

    /// Opens a TLS connection over TCP to a remote host.
    [[nodiscard]] static std::tuple<TLSStream, std::unique_ptr<Error>>
    Connect(IOService* service, std::string_view address);

    /// Opens a TLS connection over TCP to a remote host.
    [[nodiscard]] static std::tuple<TLSStream, std::unique_ptr<Error>>
    Connect(IOService* service, std::string_view address, const Duration& timeout, const ArrayView<std::uint8_t const>& certPEM);

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
    std::unique_ptr<Detail::NativeTLSStream> nativeStream;
};

} // namespace Pomdog
