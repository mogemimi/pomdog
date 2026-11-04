// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/types.h"
#include "pomdog/network/mbedtls/trust_store_mbedtls.h"
#include "pomdog/network/stream.h"
#include "pomdog/utility/errors.h"

#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/winsock_win32.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <chrono>
#include <deque>
#include <expected>
#include <future>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

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

namespace pomdog::network::detail {

/// TLSStreamMbedTLS is a TLS client stream built on Mbed TLS.
///
/// Mbed TLS keeps global PSA crypto state that is not thread-safe without
/// MBEDTLS_THREADING_C, so every Mbed TLS call runs on the thread that calls
/// `update()`. Only the blocking socket connection runs on a worker thread.
class TLSStreamMbedTLS final : public Stream {
public:
    TLSStreamMbedTLS() noexcept;

    ~TLSStreamMbedTLS() override;

    /// Sets up TLS for host and starts connecting to host on port. The
    /// handshake verifies the server against trustStore.
    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    connect(
        std::string_view host,
        std::string_view port,
        const StreamOptions& options,
        std::shared_ptr<TrustStoreMbedTLS>&& trustStore);

    void update() override;

    [[nodiscard]] std::optional<StreamEvent>
    pollEvent() override;

    [[nodiscard]] std::expected<void, std::unique_ptr<Error>>
    write(std::span<const u8> data) override;

    void close() override;

    [[nodiscard]] bool
    isConnected() const noexcept override;

private:
    enum class State : u8 {
        Connecting,
        Handshaking,
        Connected,
        Closed,
    };

    /// Takes the result of the socket connection and starts the handshake.
    void takeConnectedSocket();

    /// Advances the non-blocking handshake by one step.
    void handshake();

    /// Sends the bytes in sendBuffer_ until the connection takes no more.
    void flushSendBuffer();

    void readSocket();

    /// Closes the connection and queues a Disconnected event with err.
    void disconnect(std::unique_ptr<Error>&& err);

    void closeConnection() noexcept;

private:
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    WinsockLibrary winsock_;
#endif
    ::mbedtls_net_context descriptor_;
    ::mbedtls_ssl_context ssl_;
    ::mbedtls_ssl_config sslConfig_;

    // NOTE: sslConfig_ points to the certificates of the trust store.
    std::shared_ptr<TrustStoreMbedTLS> trustStore_;

    std::future<std::expected<int, std::unique_ptr<Error>>> connectResult_;
    std::deque<StreamEvent> events_;

    /// The bytes that write() took and the connection has not sent yet.
    std::vector<u8> sendBuffer_;

    /// The size of the last mbedtls_ssl_write() call that returned
    /// MBEDTLS_ERR_SSL_WANT_WRITE, or zero.
    std::size_t pendingWriteSize_ = 0;

    Duration connectTimeout_ = Duration::zero();
    Duration idleTimeout_ = Duration::zero();
    std::chrono::steady_clock::time_point handshakeStartTime_;
    std::chrono::steady_clock::time_point lastActiveTime_;
    State state_ = State::Closed;
};

} // namespace pomdog::network::detail
