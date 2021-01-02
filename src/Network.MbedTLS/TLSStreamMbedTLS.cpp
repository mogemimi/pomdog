// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "TLSStreamMbedTLS.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
#include "../Network.POSIX/SocketHelperPOSIX.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../Network.Win32/SocketHelperWin32.hpp"
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

#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {
namespace {

std::string MbedTLSErrorToString(int err)
{
    std::array<char, 128> buf;
    mbedtls_strerror(err, buf.data(), buf.size());
    buf.back() = 0;
    return buf.data();
}

} // namespace

TLSStreamMbedTLS::TLSStreamMbedTLS(IOService* serviceIn)
    : service(serviceIn)
{
    // NOTE: Initialize the RNG and the session data
    mbedtls_net_init(&descriptor);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&sslConfig);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctrDrbg);

    mbedtls_entropy_init(&entropy);
}

TLSStreamMbedTLS::~TLSStreamMbedTLS()
{
    if (blockingThread.joinable()) {
        blockingThread.join();
    }

    this->errorConn.Disconnect();
    this->eventLoopConn.Disconnect();

    if (this->isConnected) {
        mbedtls_ssl_close_notify(&ssl);
        this->isConnected = false;
    }

    // NOTE: destroy
    mbedtls_net_free(&descriptor);

    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&sslConfig);
    mbedtls_ctr_drbg_free(&ctrDrbg);
    mbedtls_entropy_free(&entropy);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::Connect(
    std::string_view host,
    std::string_view port,
    const Duration& connectTimeoutIn,
    const ArrayView<std::uint8_t const>& certPEM)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: Seeding the random number generator
    const char* pers = "ssl_client1";

    if (auto ret = mbedtls_ctr_drbg_seed(
            &ctrDrbg,
            mbedtls_entropy_func,
            &entropy,
            reinterpret_cast<const unsigned char*>(pers),
            std::strlen(pers)); ret != 0) {
        auto err = Errors::New("mbedtls_ctr_drbg_seed failed, " + MbedTLSErrorToString(ret));
        std::shared_ptr<Error> shared = err->Clone();
        errorConn = service->ScheduleTask([this, err = std::move(shared)] {
            this->OnConnected(err->Clone());
            this->errorConn.Disconnect();
        });
        return err;
    }

    if (!certPEM.IsEmpty() && (certPEM.GetBack() != 0)) {
        auto err = Errors::New("Certificates (PEM) must be null-terminated string");
        std::shared_ptr<Error> shared = err->Clone();
        errorConn = service->ScheduleTask([this, err = std::move(shared)] {
            this->OnConnected(err->Clone());
            this->errorConn.Disconnect();
        });
        return err;
    }

    // NOTE: Initialize certificates
    if (auto ret = mbedtls_x509_crt_parse(
            &cacert,
            reinterpret_cast<const unsigned char*>(certPEM.GetData()),
            certPEM.GetSize()); ret < 0) {
        auto err = Errors::New("mbedtls_x509_crt_parse failed, " + MbedTLSErrorToString(ret));
        std::shared_ptr<Error> shared = err->Clone();
        errorConn = service->ScheduleTask([this, err = std::move(shared)] {
            this->OnConnected(err->Clone());
            this->errorConn.Disconnect();
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
                auto err = Errors::New("mbedtls_net_connect failed, " + MbedTLSErrorToString(ret));
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }
#else

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
        const auto ConnectSocketFunc = ConnectSocketPOSIX;
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
        const auto ConnectSocketFunc = ConnectSocketWin32;
#endif

        // NOTE: Start the connection
        auto [fd, err] = ConnectSocketFunc(hostBuf, portBuf, SocketProtocol::TCP, connectTimeoutIn);

        if (err != nullptr) {
            std::shared_ptr<Error> shared = std::move(err);
            errorConn = service->ScheduleTask([this, err = std::move(shared)] {
                this->OnConnected(err->Clone());
                this->errorConn.Disconnect();
            });
            return;
        }
        this->descriptor.fd = static_cast<int>(fd);
        int ret = 0;
#endif
        // NOTE: Set up the SSL/TLS structure
        ret = mbedtls_ssl_config_defaults(
            &sslConfig,
            MBEDTLS_SSL_IS_CLIENT,
            MBEDTLS_SSL_TRANSPORT_STREAM,
            MBEDTLS_SSL_PRESET_DEFAULT);

        if (ret != 0) {
            errorConn = service->ScheduleTask([this, ret = ret] {
                auto err = Errors::New("mbedtls_ssl_config_defaults failed, " + MbedTLSErrorToString(ret));
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }

        // FIXME: OPTIONAL is not optimal for security, but makes interop easier in this simplified example
        mbedtls_ssl_conf_authmode(&sslConfig, MBEDTLS_SSL_VERIFY_OPTIONAL);
        mbedtls_ssl_conf_ca_chain(&sslConfig, &cacert, nullptr);
        mbedtls_ssl_conf_rng(&sslConfig, mbedtls_ctr_drbg_random, &ctrDrbg);
#if defined(DEBUG) && !defined(NDEBUG)
        mbedtls_ssl_conf_dbg(
            &sslConfig,
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

        ret = mbedtls_ssl_setup(&ssl, &sslConfig);
        if (ret != 0) {
            errorConn = service->ScheduleTask([this, ret = ret] {
                auto err = Errors::New("mbedtls_ssl_setup failed, " + MbedTLSErrorToString(ret));
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }

        ret = mbedtls_ssl_set_hostname(&ssl, hostBuf.data());
        if (ret != 0) {
            errorConn = service->ScheduleTask([this, ret = ret] {
                auto err = Errors::New("mbedtls_ssl_set_hostname failed, " + MbedTLSErrorToString(ret));
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }

        mbedtls_ssl_set_bio(&ssl, &descriptor, mbedtls_net_send, mbedtls_net_recv, nullptr);

        // NOTE: Perform the SSL/TLS handshake
        for (;;) {
            ret = mbedtls_ssl_handshake(&ssl);
            if (ret == 0) {
                break;
            }

            if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
                errorConn = service->ScheduleTask([this, ret = ret] {
                    auto err = Errors::New("mbedtls_ssl_handshake failed, " + MbedTLSErrorToString(ret));
                    this->OnConnected(std::move(err));
                    this->errorConn.Disconnect();
                });
                return;
            }
        }

        // NOTE: Verify the server certificate
        if (auto flags = mbedtls_ssl_get_verify_result(&ssl); flags != 0) {
            // NOTE: In real life, we probably want to bail out when ret != 0
            std::array<char, 512> buf;
            mbedtls_x509_crt_verify_info(buf.data(), buf.size(), "  ! ", flags);
            buf.back() = 0;
#if defined(DEBUG) && !defined(NDEBUG)
            auto e = Errors::New(std::string{"mbedtls_ssl_get_verify_result failed, "} + buf.data());
            std::shared_ptr<Error> shared = std::move(e);
            errorConn = service->ScheduleTask([this, err = std::move(shared)] {
                this->OnConnected(err->Clone());
                this->errorConn.Disconnect();
            });
#endif
        }

        mbedtls_net_set_nonblock(&descriptor);

        this->isConnected = true;

        eventLoopConn = service->ScheduleTask([this] {
            // NOTE: Update timestamp of last connection
            this->lastActiveTime = this->service->GetNowTime();

            this->OnConnected(nullptr);
            eventLoopConn.Disconnect();
            eventLoopConn = service->ScheduleTask([this] { this->ReadEventLoop(); });
        });
    });

    this->blockingThread = std::move(connectThread);

    return nullptr;
}

void TLSStreamMbedTLS::Close()
{
    this->isConnected = false;
    eventLoopConn.Disconnect();
    mbedtls_ssl_close_notify(&ssl);
}

std::unique_ptr<Error>
TLSStreamMbedTLS::Write(const ArrayView<std::uint8_t const>& data)
{
    for (;;) {
        auto ret = mbedtls_ssl_write(&ssl, data.GetData(), data.GetSize());
        if (ret > 0) {
            break;
        }
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            return Errors::New("mbedtls_ssl_write failed, " + MbedTLSErrorToString(ret));
        }
    }

    // NOTE: Update timestamp of last read/write
    this->lastActiveTime = service->GetNowTime();

    return nullptr;
}

bool TLSStreamMbedTLS::IsConnected() const noexcept
{
    return this->isConnected;
}

void TLSStreamMbedTLS::SetTimeout(const Duration& timeoutIn)
{
    POMDOG_ASSERT(timeoutIn >= std::remove_reference_t<decltype(timeoutIn)>::zero());
    this->timeoutInterval = timeoutIn;
}

void TLSStreamMbedTLS::ReadEventLoop()
{
    if (timeoutInterval.has_value()) {
        if ((service->GetNowTime() - lastActiveTime) > *timeoutInterval) {
            this->OnRead({}, Errors::New("timeout socket connection"));
            this->Close();
            this->OnDisconnect();
            return;
        }
    }

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 2048 bytes.
    std::array<std::uint8_t, 2048> buffer;

    auto ret = mbedtls_ssl_read(&ssl, reinterpret_cast<unsigned char*>(buffer.data()), buffer.size());
    if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        return;
    }

    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        this->Close();
        this->OnDisconnect();
        return;
    }
    if (ret < 0) {
        this->OnRead({}, Errors::New("mbedtls_ssl_read failed, " + MbedTLSErrorToString(ret)));
        return;
    }

    // NOTE: Update timestamp of last read/write
    this->lastActiveTime = service->GetNowTime();

    const auto readSize = static_cast<size_t>(ret);

    // NOTE: When the readSize is zero, it means EOF.
    static_assert(std::is_unsigned_v<decltype(readSize)>, "readSize >= 0");

    auto view = ArrayView<std::uint8_t>{buffer.data(), readSize};
    this->OnRead(std::move(view), nullptr);
}

} // namespace Pomdog::Detail
