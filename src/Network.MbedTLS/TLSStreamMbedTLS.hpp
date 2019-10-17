// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/TimePoint.hpp"
#include "Pomdog/Network/detail/ForwardDeclarations.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <string_view>
#include <thread>

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#endif
#include "mbedtls/certs.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

namespace Pomdog::Detail {

class TLSStreamMbedTLS final {
public:
    explicit TLSStreamMbedTLS(IOService* serviceIn);

    ~TLSStreamMbedTLS();

    /// Opens a TCP connection over TCP to a remote host.
    [[nodiscard]] std::shared_ptr<Error>
    Connect(const std::string_view& host, const std::string_view& port, const Duration& timeout, const ArrayView<std::uint8_t const>& certPEM);

    /// Closes the connection.
    void Close();

    /// Writes data to the connection.
    [[nodiscard]] std::shared_ptr<Error>
    Write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool IsConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void SetTimeout(const Duration& timeout);

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::shared_ptr<Error>&)> OnConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> OnDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::shared_ptr<Error>&)> OnRead;

private:
    void ReadEventLoop();

private:
    IOService* service = nullptr;

    ::mbedtls_net_context descriptor;
    ::mbedtls_entropy_context entropy;
    ::mbedtls_ctr_drbg_context ctrDrbg;
    ::mbedtls_ssl_context ssl;
    ::mbedtls_ssl_config sslConfig;
    ::mbedtls_x509_crt cacert;

    ScopedConnection eventLoopConn;
    ScopedConnection errorConn;

    std::thread blockingThread;
    std::optional<Duration> timeoutInterval;
    TimePoint lastActiveTime;
    std::atomic<bool> isConnected = false;
};

} // namespace Pomdog::Detail
