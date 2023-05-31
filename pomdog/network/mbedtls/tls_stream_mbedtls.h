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

namespace pomdog::detail {

class TLSStreamMbedTLS final {
public:
    explicit TLSStreamMbedTLS(IOService* serviceIn);

    ~TLSStreamMbedTLS();

    /// Opens a TCP connection over TCP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    connect(std::string_view host, std::string_view port, const Duration& timeout, const ArrayView<std::uint8_t const>& certPEM);

    /// Closes the connection.
    void close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(const ArrayView<std::uint8_t const>& data);

    /// @return True if the socket is connected to a remote host, false otherwise.
    [[nodiscard]] bool
    isConnected() const noexcept;

    /// Sets the interval to wait for socket activity.
    void setTimeout(const Duration& timeout);

    /// Delegate that fires when a connection is successfully established.
    Delegate<void(const std::unique_ptr<Error>&)> onConnected;

    /// Delegate that fires when a connection is disconnected.
    Delegate<void()> onDisconnect;

    /// Delegate that fires when a data packet is received.
    Delegate<void(const ArrayView<std::uint8_t>&, const std::unique_ptr<Error>&)> onRead;

private:
    void readEventLoop();

private:
    IOService* service_ = nullptr;

    ::mbedtls_net_context descriptor_;
    ::mbedtls_entropy_context entropy_;
    ::mbedtls_ctr_drbg_context ctrDrbg_;
    ::mbedtls_ssl_context ssl_;
    ::mbedtls_ssl_config sslConfig_;
    ::mbedtls_x509_crt cacert_;

    ScopedConnection eventLoopConn_;
    ScopedConnection errorConn_;

    std::thread blockingThread_;
    std::optional<Duration> timeoutInterval_;
    TimePoint lastActiveTime_;
    std::atomic<bool> isConnected_ = false;
};

} // namespace pomdog::detail
