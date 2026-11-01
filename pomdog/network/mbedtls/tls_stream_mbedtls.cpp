// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/io_service.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

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
#include <chrono>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

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

TLSStreamMbedTLS::TLSStreamMbedTLS(IOService* serviceIn)
    : service_(serviceIn)
{
    mbedtls_net_init(&descriptor_);
    mbedtls_ssl_init(&ssl_);
    mbedtls_ssl_config_init(&sslConfig_);
    mbedtls_x509_crt_init(&cacert_);
}

TLSStreamMbedTLS::~TLSStreamMbedTLS()
{
    errorConn_.disconnect();
    eventLoopConn_.disconnect();

    // NOTE: Wait for a socket connection in progress and adopt its socket, so
    // that mbedtls_net_free() closes it.
    if (connectResult_.valid()) {
        [[maybe_unused]] const auto connectErr = takeConnectedSocket();
    }

    if (isConnected_) {
        mbedtls_ssl_close_notify(&ssl_);
        isConnected_ = false;
    }

    mbedtls_net_free(&descriptor_);
    mbedtls_x509_crt_free(&cacert_);
    mbedtls_ssl_free(&ssl_);
    mbedtls_ssl_config_free(&sslConfig_);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::connect(
    std::string_view host,
    std::string_view port,
    const Duration& connectTimeoutIn,
    std::span<const u8> certPEM)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: Reports a setup failure through onConnected as well as the return value.
    const auto fail = [this](std::unique_ptr<Error> err) -> std::unique_ptr<Error> {
        std::shared_ptr<Error> shared = err->clone();
        errorConn_ = service_->scheduleTask([this, shared = std::move(shared)] {
            onConnected(shared->clone());
            errorConn_.disconnect();
        });
        return err;
    };

    // NOTE: Certificate parsing and TLS use the PSA random generator.
    // psa_crypto_init() succeeds again when PSA is already initialized, and
    // PSA stays initialized for the process lifetime.
    if (auto status = psa_crypto_init(); status != psaSuccess) {
        return fail(errors::make("psa_crypto_init failed, " + mbedTLSErrorToString(status)));
    }

    if (!certPEM.empty() && (certPEM.back() != 0)) {
        return fail(errors::make("Certificates (PEM) must be null-terminated string"));
    }

    // NOTE: Initialize certificates
    if (auto ret = mbedtls_x509_crt_parse(
            &cacert_,
            reinterpret_cast<const unsigned char*>(certPEM.data()),
            certPEM.size());
        ret < 0) {
        return fail(errors::make("mbedtls_x509_crt_parse failed, " + mbedTLSErrorToString(ret)));
    }

    // NOTE: Set up the SSL/TLS structure
    if (auto ret = mbedtls_ssl_config_defaults(
            &sslConfig_,
            MBEDTLS_SSL_IS_CLIENT,
            MBEDTLS_SSL_TRANSPORT_STREAM,
            MBEDTLS_SSL_PRESET_DEFAULT);
        ret != 0) {
        return fail(errors::make("mbedtls_ssl_config_defaults failed, " + mbedTLSErrorToString(ret)));
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
        return fail(errors::make("mbedtls_ssl_setup failed, " + mbedTLSErrorToString(ret)));
    }

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    auto hostBuf = std::string{host};
    auto portBuf = std::string{port};

    if (auto ret = mbedtls_ssl_set_hostname(&ssl_, hostBuf.data()); ret != 0) {
        return fail(errors::make("mbedtls_ssl_set_hostname failed, " + mbedTLSErrorToString(ret)));
    }

    // NOTE: Only the blocking socket connection runs on a worker thread; it
    // does not call Mbed TLS.
    connectResult_ = std::async(
        std::launch::async,
        [hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeoutIn] {
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
            auto [fd, err] = connectSocketPOSIX(hostBuf, portBuf, SocketProtocol::TCP, connectTimeoutIn);
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
            auto [fd, err] = connectSocketWin32(hostBuf, portBuf, SocketProtocol::TCP, connectTimeoutIn);
#endif
            return std::make_tuple(static_cast<int>(fd), std::move(err));
        });

    connectTimeout_ = connectTimeoutIn;
    eventLoopConn_ = service_->scheduleTask([this] { connectEventLoop(); });

    return nullptr;
}

std::unique_ptr<Error>
TLSStreamMbedTLS::takeConnectedSocket()
{
    POMDOG_ASSERT(connectResult_.valid());
    auto [fd, err] = connectResult_.get();
    if (err != nullptr) {
        return std::move(err);
    }
    descriptor_.fd = fd;
    return nullptr;
}

void TLSStreamMbedTLS::connectEventLoop()
{
    POMDOG_ASSERT(connectResult_.valid());
    if (connectResult_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
        return;
    }
    eventLoopConn_.disconnect();

    if (auto err = takeConnectedSocket(); err != nullptr) {
        onConnected(std::move(err));
        return;
    }

    if (auto ret = mbedtls_net_set_nonblock(&descriptor_); ret != 0) {
        onConnected(errors::make("mbedtls_net_set_nonblock failed, " + mbedTLSErrorToString(ret)));
        return;
    }
    mbedtls_ssl_set_bio(&ssl_, &descriptor_, mbedtls_net_send, mbedtls_net_recv, nullptr);

    handshakeStartTime_ = service_->getNowTime();
    eventLoopConn_ = service_->scheduleTask([this] { handshakeEventLoop(); });
}

void TLSStreamMbedTLS::handshakeEventLoop()
{
    // NOTE: Perform the SSL/TLS handshake
    const auto ret = mbedtls_ssl_handshake(&ssl_);
    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        if ((service_->getNowTime() - handshakeStartTime_) > connectTimeout_) {
            eventLoopConn_.disconnect();
            onConnected(errors::make("TLS handshake timed out"));
        }
        return;
    }
    eventLoopConn_.disconnect();

    if (ret != 0) {
        onConnected(errors::make("mbedtls_ssl_handshake failed, " + mbedTLSErrorToString(ret)));
        return;
    }

    isConnected_ = true;

    // NOTE: Update timestamp of last connection
    lastActiveTime_ = service_->getNowTime();

    // NOTE: Start reading before notifying, so that a callback that closes
    // the stream also stops the read loop.
    eventLoopConn_ = service_->scheduleTask([this] { readEventLoop(); });

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    // NOTE: Verify the server certificate. In real life, we probably want to
    // bail out when the verification fails.
    if (auto flags = mbedtls_ssl_get_verify_result(&ssl_); flags != 0) {
        std::array<char, 512> buf;
        mbedtls_x509_crt_verify_info(buf.data(), buf.size(), "  ! ", flags);
        buf.back() = 0;
        onConnected(errors::make(std::string{"mbedtls_ssl_get_verify_result failed, "} + buf.data()));
    }
#endif

    onConnected(nullptr);
}

void TLSStreamMbedTLS::close()
{
    isConnected_ = false;
    eventLoopConn_.disconnect();
    mbedtls_ssl_close_notify(&ssl_);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::write(std::span<const u8> data)
{
    for (;;) {
        auto ret = mbedtls_ssl_write(&ssl_, data.data(), data.size());
        if (ret > 0) {
            break;
        }
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return errors::make("mbedtls_ssl_write failed, " + mbedTLSErrorToString(ret));
        }
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    return nullptr;
}

bool TLSStreamMbedTLS::isConnected() const noexcept
{
    return isConnected_;
}

void TLSStreamMbedTLS::setTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    timeoutInterval_ = timeoutIn;
}

void TLSStreamMbedTLS::readEventLoop()
{
    if (timeoutInterval_.has_value()) {
        if ((service_->getNowTime() - lastActiveTime_) > *timeoutInterval_) {
            onRead({}, errors::make("timeout socket connection"));
            close();
            onDisconnect();
            return;
        }
    }

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

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        close();
        onDisconnect();
        return;
    }
    if (ret < 0) {
        onRead({}, errors::make("mbedtls_ssl_read failed, " + mbedTLSErrorToString(ret)));
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    const auto readSize = static_cast<size_t>(ret);

    // NOTE: When the readSize is zero, it means EOF.
    static_assert(std::is_unsigned_v<decltype(readSize)>, "readSize >= 0");

    if (readSize > static_cast<size_t>(buffer.size())) {
        onRead({}, errors::make("read size is larger than buffer size"));
        return;
    }

    POMDOG_ASSERT(readSize <= static_cast<size_t>(buffer.size()));
    POMDOG_CLANG_UNSAFE_BUFFER_BEGIN
    auto view = std::span<u8>{buffer.data(), readSize};
    POMDOG_CLANG_UNSAFE_BUFFER_END
    onRead(std::move(view), nullptr);
}

} // namespace pomdog::detail
