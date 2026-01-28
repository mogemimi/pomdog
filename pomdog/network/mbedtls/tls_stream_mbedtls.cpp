// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/array_view.h"
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
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#include <algorithm>
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

std::string MbedTLSErrorToString(int err)
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
    // NOTE: Initialize the RNG and the session data
    mbedtls_net_init(&descriptor_);
    mbedtls_ssl_init(&ssl_);
    mbedtls_ssl_config_init(&sslConfig_);
    mbedtls_x509_crt_init(&cacert_);
    mbedtls_ctr_drbg_init(&ctrDrbg_);

    mbedtls_entropy_init(&entropy_);
}

TLSStreamMbedTLS::~TLSStreamMbedTLS()
{
    if (blockingThread_.joinable()) {
        blockingThread_.join();
    }

    errorConn_.disconnect();
    eventLoopConn_.disconnect();

    if (isConnected_) {
        mbedtls_ssl_close_notify(&ssl_);
        isConnected_ = false;
    }

    // NOTE: destroy
    mbedtls_net_free(&descriptor_);

    mbedtls_x509_crt_free(&cacert_);
    mbedtls_ssl_free(&ssl_);
    mbedtls_ssl_config_free(&sslConfig_);
    mbedtls_ctr_drbg_free(&ctrDrbg_);
    mbedtls_entropy_free(&entropy_);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::connect(
    std::string_view host,
    std::string_view port,
    const Duration& connectTimeoutIn,
    const ArrayView<std::uint8_t const>& certPEM)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: Seeding the random number generator
    const char* pers = "ssl_client1";

    if (auto ret = mbedtls_ctr_drbg_seed(
            &ctrDrbg_,
            mbedtls_entropy_func,
            &entropy_,
            reinterpret_cast<const unsigned char*>(pers),
            std::strlen(pers));
        ret != 0) {
        auto err = errors::make("mbedtls_ctr_drbg_seed failed, " + MbedTLSErrorToString(ret));
        std::shared_ptr<Error> shared = err->clone();
        errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
            onConnected(err->clone());
            errorConn_.disconnect();
        });
        return err;
    }

    if (!certPEM.empty() && (certPEM.back() != 0)) {
        auto err = errors::make("Certificates (PEM) must be null-terminated string");
        std::shared_ptr<Error> shared = err->clone();
        errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
            onConnected(err->clone());
            errorConn_.disconnect();
        });
        return err;
    }

    // NOTE: Initialize certificates
    if (auto ret = mbedtls_x509_crt_parse(
            &cacert_,
            reinterpret_cast<const unsigned char*>(certPEM.data()),
            certPEM.size());
        ret < 0) {
        auto err = errors::make("mbedtls_x509_crt_parse failed, " + MbedTLSErrorToString(ret));
        std::shared_ptr<Error> shared = err->clone();
        errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
            onConnected(err->clone());
            errorConn_.disconnect();
        });
        return err;
    }

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeoutIn] {
#if 0
        // NOTE: Start the connection
        auto ret = mbedtls_net_connect(&descriptor, hostBuf.data(), portBuf.data(), MBEDTLS_NET_PROTO_TCP);
        if (ret != 0) {
            errorConn = service->ScheduleTask([this, ret = ret] {
                auto err = errors::make("mbedtls_net_connect failed, " + MbedTLSErrorToString(ret));
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }
#else

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
        const auto connectSocketFunc = connectSocketPOSIX;
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        const auto connectSocketFunc = connectSocketWin32;
#endif

        // NOTE: Start the connection
        auto [fd, connectErr] = connectSocketFunc(hostBuf, portBuf, SocketProtocol::TCP, connectTimeoutIn);

        if (connectErr != nullptr) {
            std::shared_ptr<Error> shared = std::move(connectErr);
            errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
                onConnected(err->clone());
                errorConn_.disconnect();
            });
            return;
        }
        descriptor_.fd = static_cast<int>(fd);
        int ret = 0;
#endif
        // NOTE: Set up the SSL/TLS structure
        ret = mbedtls_ssl_config_defaults(
            &sslConfig_,
            MBEDTLS_SSL_IS_CLIENT,
            MBEDTLS_SSL_TRANSPORT_STREAM,
            MBEDTLS_SSL_PRESET_DEFAULT);

        if (ret != 0) {
            errorConn_ = service_->scheduleTask([this, ret = ret] {
                auto err = errors::make("mbedtls_ssl_config_defaults failed, " + MbedTLSErrorToString(ret));
                onConnected(std::move(err));
                errorConn_.disconnect();
            });
            return;
        }

        // FIXME: OPTIONAL is not optimal for security, but makes interop easier in this simplified example
        mbedtls_ssl_conf_authmode(&sslConfig_, MBEDTLS_SSL_VERIFY_OPTIONAL);
        mbedtls_ssl_conf_ca_chain(&sslConfig_, &cacert_, nullptr);
        mbedtls_ssl_conf_rng(&sslConfig_, mbedtls_ctr_drbg_random, &ctrDrbg_);
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
        mbedtls_ssl_conf_dbg(
            &sslConfig_,
            [](void* ctx, [[maybe_unused]] int level, const char* file, int line, const char* str) {
#if defined(_MSC_VER) && (_MSC_VER < 1920)
                // NOTE: Visual Studio 2017 and older don't fully support
                // [[maybe_unused]] attribute, and causes error C4100.
                (void)level;
#endif
                ::fprintf(reinterpret_cast<FILE*>(ctx), "%s:%04d: %s", file, line, str);
                fflush(reinterpret_cast<FILE*>(ctx));
            },
            stdout);
#endif

        ret = mbedtls_ssl_setup(&ssl_, &sslConfig_);
        if (ret != 0) {
            errorConn_ = service_->scheduleTask([this, ret = ret] {
                auto err = errors::make("mbedtls_ssl_setup failed, " + MbedTLSErrorToString(ret));
                onConnected(std::move(err));
                errorConn_.disconnect();
            });
            return;
        }

        ret = mbedtls_ssl_set_hostname(&ssl_, hostBuf.data());
        if (ret != 0) {
            errorConn_ = service_->scheduleTask([this, ret = ret] {
                auto err = errors::make("mbedtls_ssl_set_hostname failed, " + MbedTLSErrorToString(ret));
                onConnected(std::move(err));
                errorConn_.disconnect();
            });
            return;
        }

        mbedtls_ssl_set_bio(&ssl_, &descriptor_, mbedtls_net_send, mbedtls_net_recv, nullptr);

        // NOTE: Perform the SSL/TLS handshake
        for (;;) {
            ret = mbedtls_ssl_handshake(&ssl_);
            if (ret == 0) {
                break;
            }

            if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
                errorConn_ = service_->scheduleTask([this, ret = ret] {
                    auto err = errors::make("mbedtls_ssl_handshake failed, " + MbedTLSErrorToString(ret));
                    onConnected(std::move(err));
                    errorConn_.disconnect();
                });
                return;
            }
        }

        // NOTE: Verify the server certificate
        if (auto flags = mbedtls_ssl_get_verify_result(&ssl_); flags != 0) {
            // NOTE: In real life, we probably want to bail out when ret != 0
            std::array<char, 512> buf;
            mbedtls_x509_crt_verify_info(buf.data(), buf.size(), "  ! ", flags);
            buf.back() = 0;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
            auto e = errors::make(std::string{"mbedtls_ssl_get_verify_result failed, "} + buf.data());
            std::shared_ptr<Error> shared = std::move(e);
            errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
                onConnected(err->clone());
                errorConn_.disconnect();
            });
#endif
        }

        mbedtls_net_set_nonblock(&descriptor_);

        isConnected_ = true;

        eventLoopConn_ = service_->scheduleTask([this] {
            // NOTE: Update timestamp of last connection
            lastActiveTime_ = service_->getNowTime();

            onConnected(nullptr);
            eventLoopConn_.disconnect();
            eventLoopConn_ = service_->scheduleTask([this] { readEventLoop(); });
        });
    });

    blockingThread_ = std::move(connectThread);

    return nullptr;
}

void TLSStreamMbedTLS::close()
{
    isConnected_ = false;
    eventLoopConn_.disconnect();
    mbedtls_ssl_close_notify(&ssl_);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::write(const ArrayView<std::uint8_t const>& data)
{
    for (;;) {
        auto ret = mbedtls_ssl_write(&ssl_, data.data(), data.size());
        if (ret > 0) {
            break;
        }
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return errors::make("mbedtls_ssl_write failed, " + MbedTLSErrorToString(ret));
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
    std::array<std::uint8_t, 2048> buffer;

    auto ret = mbedtls_ssl_read(&ssl_, reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        return;
    }

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        close();
        onDisconnect();
        return;
    }
    if (ret < 0) {
        onRead({}, errors::make("mbedtls_ssl_read failed, " + MbedTLSErrorToString(ret)));
        return;
    }

    // NOTE: Update timestamp of last read/write
    lastActiveTime_ = service_->getNowTime();

    const auto readSize = static_cast<size_t>(ret);

    // NOTE: When the readSize is zero, it means EOF.
    static_assert(std::is_unsigned_v<decltype(readSize)>, "readSize >= 0");

    auto view = ArrayView<std::uint8_t>{buffer.data(), readSize};
    onRead(std::move(view), nullptr);
}

} // namespace pomdog::detail
