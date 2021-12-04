// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Application/TimePoint.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Network/ForwardDeclarations.hpp"
#include "Pomdog/Signals/Delegate.hpp"
#include "Pomdog/Signals/ScopedConnection.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
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
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

class TLSStreamMbedTLS final {
public:
    explicit TLSStreamMbedTLS(IOService* serviceIn);

    ~TLSStreamMbedTLS();

    /// Opens a TCP connection over TCP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    Connect(std::string_view host, std::string_view port, const Duration& timeout, const ArrayView<std::uint8_t const>& certPEM);

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
