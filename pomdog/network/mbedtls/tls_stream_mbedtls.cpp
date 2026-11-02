// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/assert.h"

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/socket_helper_posix.h"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "pomdog/network/win32/socket_helper_win32.h"
#endif

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wdocumentation-unknown-command"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wredundant-decls"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#include "psa/crypto.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <array>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

// NOTE: PSA_SUCCESS expands to a C-style cast, which -Wold-style-cast rejects.
POMDOG_CLANG_SUPPRESS_WARNING_PUSH
POMDOG_CLANG_SUPPRESS_WARNING("-Wold-style-cast")
POMDOG_GCC_SUPPRESS_WARNING_PUSH
POMDOG_GCC_SUPPRESS_WARNING("-Wold-style-cast")
constexpr psa_status_t psaSuccess = PSA_SUCCESS;
POMDOG_GCC_SUPPRESS_WARNING_POP
POMDOG_CLANG_SUPPRESS_WARNING_POP

std::string mbedTLSErrorToString(int err)
{
    std::array<char, 128> buf;
    std::fill(std::begin(buf), std::end(buf), '\0');
    mbedtls_strerror(err, buf.data(), buf.size());
    buf.back() = 0;
    return buf.data();
}

} // namespace

TLSStreamMbedTLS::TLSStreamMbedTLS() noexcept
{
    mbedtls_net_init(&descriptor_);
    mbedtls_ssl_init(&ssl_);
    mbedtls_ssl_config_init(&sslConfig_);
    mbedtls_x509_crt_init(&cacert_);
}

TLSStreamMbedTLS::~TLSStreamMbedTLS()
{
    // NOTE: Wait for a socket connection in progress and adopt its socket, so
    // that mbedtls_net_free() closes it.
    if (connectResult_.valid()) {
        if (auto result = connectResult_.get(); result) {
            descriptor_.fd = *result;
        }
    }

    closeConnection();

    mbedtls_x509_crt_free(&cacert_);
    mbedtls_ssl_free(&ssl_);
    mbedtls_ssl_config_free(&sslConfig_);
}

std::expected<void, std::unique_ptr<Error>>
TLSStreamMbedTLS::connect(
    std::string_view host,
    std::string_view port,
    const StreamOptions& options,
    std::span<const u8> certPEM)
{
#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto result = winsock_.startup(); !result) {
        return std::unexpected(std::move(result.error()));
    }
#endif

    // NOTE: Certificate parsing and TLS use the PSA random generator.
    // psa_crypto_init() succeeds again when PSA is already initialized, and
    // PSA stays initialized for the process lifetime.
    if (auto status = psa_crypto_init(); status != psaSuccess) {
        return std::unexpected(errors::make("psa_crypto_init failed, " + mbedTLSErrorToString(status)));
    }

    if (!certPEM.empty() && (certPEM.back() != 0)) {
        return std::unexpected(errors::make("Certificates (PEM) must be null-terminated string"));
    }

    // NOTE: Initialize certificates
    if (auto ret = mbedtls_x509_crt_parse(
            &cacert_,
            reinterpret_cast<const unsigned char*>(certPEM.data()),
            certPEM.size());
        ret < 0) {
        return std::unexpected(errors::make("mbedtls_x509_crt_parse failed, " + mbedTLSErrorToString(ret)));
    }

    // NOTE: Set up the SSL/TLS structure
    if (auto ret = mbedtls_ssl_config_defaults(
            &sslConfig_,
            MBEDTLS_SSL_IS_CLIENT,
            MBEDTLS_SSL_TRANSPORT_STREAM,
            MBEDTLS_SSL_PRESET_DEFAULT);
        ret != 0) {
        return std::unexpected(errors::make("mbedtls_ssl_config_defaults failed, " + mbedTLSErrorToString(ret)));
    }

    // FIXME: OPTIONAL is not optimal for security, but makes interop easier in this simplified example
    mbedtls_ssl_conf_authmode(&sslConfig_, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&sslConfig_, &cacert_, nullptr);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    mbedtls_ssl_conf_dbg(
        &sslConfig_,
        [](void* ctx, [[maybe_unused]] int level, const char* file, int line, const char* str) {
            ::fprintf(reinterpret_cast<FILE*>(ctx), "%s:%04d: %s", file, line, str);
            fflush(reinterpret_cast<FILE*>(ctx));
        },
        stdout);
#endif

    if (auto ret = mbedtls_ssl_setup(&ssl_, &sslConfig_); ret != 0) {
        return std::unexpected(errors::make("mbedtls_ssl_setup failed, " + mbedTLSErrorToString(ret)));
    }

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    auto hostBuf = std::string{host};
    auto portBuf = std::string{port};

    if (auto ret = mbedtls_ssl_set_hostname(&ssl_, hostBuf.data()); ret != 0) {
        return std::unexpected(errors::make("mbedtls_ssl_set_hostname failed, " + mbedTLSErrorToString(ret)));
    }

    connectTimeout_ = options.connectTimeout;
    idleTimeout_ = options.idleTimeout;

    // NOTE: Only the blocking socket connection runs on a worker thread; it
    // does not call Mbed TLS.
    connectResult_ = std::async(
        std::launch::async,
        [hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = options.connectTimeout]() -> std::expected<int, std::unique_ptr<Error>> {
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
            auto result = connectSocketPOSIX(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
            auto result = connectSocketWin32(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);
#endif
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TLS socket on " + hostBuf + ":" + portBuf));
            }
            return static_cast<int>(*result);
        });

    state_ = State::Connecting;

    return {};
}

void TLSStreamMbedTLS::update()
{
    if (connectResult_.valid()) {
        if (connectResult_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
            return;
        }
        takeConnectedSocket();
    }

    if (state_ == State::Handshaking) {
        handshake();
    }

    if (state_ == State::Connected) {
        if ((idleTimeout_ > Duration::zero()) && ((Clock::now() - lastActiveTime_) > idleTimeout_)) {
            disconnect(errors::make("timeout socket connection"));
            return;
        }
        readSocket();
    }
}

std::optional<StreamEvent>
TLSStreamMbedTLS::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

void TLSStreamMbedTLS::takeConnectedSocket()
{
    POMDOG_ASSERT(connectResult_.valid());
    auto result = connectResult_.get();

    if (state_ == State::Closed) {
        // NOTE: The stream was closed while connecting.
        if (result) {
            descriptor_.fd = *result;
            mbedtls_net_free(&descriptor_);
        }
        return;
    }

    POMDOG_ASSERT(state_ == State::Connecting);
    if (!result) {
        disconnect(std::move(result.error()));
        return;
    }
    descriptor_.fd = *result;

    if (auto ret = mbedtls_net_set_nonblock(&descriptor_); ret != 0) {
        disconnect(errors::make("mbedtls_net_set_nonblock failed, " + mbedTLSErrorToString(ret)));
        return;
    }
    mbedtls_ssl_set_bio(&ssl_, &descriptor_, mbedtls_net_send, mbedtls_net_recv, nullptr);

    handshakeStartTime_ = Clock::now();
    state_ = State::Handshaking;
}

void TLSStreamMbedTLS::handshake()
{
    POMDOG_ASSERT(state_ == State::Handshaking);

    // NOTE: Perform the SSL/TLS handshake
    const auto ret = mbedtls_ssl_handshake(&ssl_);
    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        if ((Clock::now() - handshakeStartTime_) > connectTimeout_) {
            disconnect(errors::make("TLS handshake timed out"));
        }
        return;
    }

    if (ret != 0) {
        disconnect(errors::make("mbedtls_ssl_handshake failed, " + mbedTLSErrorToString(ret)));
        return;
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    // NOTE: Verify the server certificate. Release builds still accept a
    // certificate that fails verification.
    if (auto flags = mbedtls_ssl_get_verify_result(&ssl_); flags != 0) {
        std::array<char, 512> buf;
        mbedtls_x509_crt_verify_info(buf.data(), buf.size(), "  ! ", flags);
        buf.back() = 0;
        disconnect(errors::make(std::string{"mbedtls_ssl_get_verify_result failed, "} + buf.data()));
        return;
    }
#endif

    state_ = State::Connected;

    // NOTE: Update timestamp of last connection
    lastActiveTime_ = Clock::now();

    events_.push_back(StreamEvent{.kind = StreamEventKind::Connected, .data = {}, .error = nullptr});
}

std::expected<void, std::unique_ptr<Error>>
TLSStreamMbedTLS::write(std::span<const u8> data)
{
    if (state_ != State::Connected) {
        return std::unexpected(errors::make("the TLS stream is not connected"));
    }
    if (data.empty()) {
        return {};
    }

    for (;;) {
        auto ret = mbedtls_ssl_write(&ssl_, data.data(), data.size());
        if (ret > 0) {
            break;
        }
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return std::unexpected(errors::make("mbedtls_ssl_write failed, " + mbedTLSErrorToString(ret)));
        }
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = Clock::now();

    return {};
}

void TLSStreamMbedTLS::close()
{
    closeConnection();
    state_ = State::Closed;
    events_.clear();
}

bool TLSStreamMbedTLS::isConnected() const noexcept
{
    return state_ == State::Connected;
}

void TLSStreamMbedTLS::disconnect(std::unique_ptr<Error>&& err)
{
    closeConnection();
    state_ = State::Closed;
    events_.push_back(StreamEvent{.kind = StreamEventKind::Disconnected, .data = {}, .error = std::move(err)});
}

void TLSStreamMbedTLS::closeConnection() noexcept
{
    if (state_ == State::Connected) {
        mbedtls_ssl_close_notify(&ssl_);
    }
    mbedtls_net_free(&descriptor_);
}

void TLSStreamMbedTLS::readSocket()
{
    POMDOG_ASSERT(state_ == State::Connected);

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<u8, 2048> buffer;

    auto ret = mbedtls_ssl_read(&ssl_, reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());

    // NOTE: A TLS 1.3 server may send session tickets after the handshake.
    // Mbed TLS reports each one before returning application data.
    while (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
        ret = mbedtls_ssl_read(&ssl_, reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
    }

    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        return;
    }

    if ((ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) || (ret == 0)) {
        // NOTE: Zero means that the peer closed the connection without
        // sending close_notify.
        disconnect(nullptr);
        return;
    }
    if (ret < 0) {
        disconnect(errors::make("mbedtls_ssl_read failed, " + mbedTLSErrorToString(ret)));
        return;
    }

    const auto readSize = static_cast<std::size_t>(ret);
    if (readSize > buffer.size()) {
        disconnect(errors::make("read size is larger than buffer size"));
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = Clock::now();

    const auto view = std::span{buffer}.first(readSize);
    events_.push_back(StreamEvent{.kind = StreamEventKind::Received, .data = std::vector<u8>(view.begin(), view.end()), .error = nullptr});
}

} // namespace pomdog::network::detail
