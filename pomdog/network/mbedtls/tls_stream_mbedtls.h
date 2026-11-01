// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/chrono/time_point.h"
#include "pomdog/network/forward_declarations.h"
#include "pomdog/signals/delegate.h"
#include "pomdog/signals/scoped_connection.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <tuple>

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4061)
POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation")
POMDOG_CLANG_SUPPRESS_WARNING("-Wdocumentation-unknown-command")
POMDOG_CLANG_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_CLANG_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
POMDOG_GCC_SUPPRESS_WARNING_PUSH
POMDOG_GCC_SUPPRESS_WARNING("-Wredundant-decls")
POMDOG_GCC_SUPPRESS_WARNING("-Wzero-as-null-pointer-constant")
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
POMDOG_GCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP
POMDOG_MSVC_SUPPRESS_WARNING_POP

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

/// TLSStreamMbedTLS is a TLS client stream built on Mbed TLS.
///
/// Mbed TLS keeps global PSA crypto state that is not thread-safe without
/// MBEDTLS_THREADING_C, so every Mbed TLS call runs on the thread that steps
/// the IOService. Only the blocking socket connection runs on a worker thread.
class TLSStreamMbedTLS final {
public:
    explicit TLSStreamMbedTLS(IOService* serviceIn);

    ~TLSStreamMbedTLS();

    /// Opens a TCP connection over TCP to a remote host.
    [[nodiscard]] std::unique_ptr<Error>
    connect(std::string_view host, std::string_view port, const Duration& timeout, std::span<const u8> certPEM);

    /// Closes the connection.
    void close();

    /// Writes data to the connection.
    [[nodiscard]] std::unique_ptr<Error>
    write(std::span<const u8> data);

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
    Delegate<void(std::span<u8>, const std::unique_ptr<Error>&)> onRead;

private:
    /// Waits for the socket connection, then starts the handshake.
    void connectEventLoop();

    /// Advances the non-blocking handshake by one step.
    void handshakeEventLoop();

    void readEventLoop();

    /// Takes the result of the socket connection and adopts its socket.
    [[nodiscard]] std::unique_ptr<Error>
    takeConnectedSocket();

private:
    IOService* service_ = nullptr;

    ::mbedtls_net_context descriptor_;
    ::mbedtls_ssl_context ssl_;
    ::mbedtls_ssl_config sslConfig_;
    ::mbedtls_x509_crt cacert_;

    ScopedConnection eventLoopConn_;
    ScopedConnection errorConn_;

    std::future<std::tuple<int, std::unique_ptr<Error>>> connectResult_;
    Duration connectTimeout_ = Duration::zero();
    TimePoint handshakeStartTime_;
    std::optional<Duration> timeoutInterval_;
    TimePoint lastActiveTime_;
    std::atomic<bool> isConnected_ = false;
};

} // namespace pomdog::detail
