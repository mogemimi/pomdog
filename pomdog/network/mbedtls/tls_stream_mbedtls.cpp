// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/mbedtls/tls_stream_mbedtls.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/mbedtls/mbedtls_helper.h"
#include "pomdog/network/receive_limits.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/assert.h"

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/utility/error_helper.h"
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
#include "mbedtls/error.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif

#if defined(POMDOG_PLATFORM_LINUX)
#include <sys/socket.h>
#include <sys/types.h>
#include <cerrno>
#endif
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include <unistd.h>
#endif

#include <algorithm>
#include <array>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

/// Returns why the server certificate failed verification, as Mbed TLS
/// reports in flags.
[[nodiscard]] std::string
verifyResultToString(u32 flags)
{
    std::array<char, 512> buf;
    buf.fill(0);
    mbedtls_x509_crt_verify_info(buf.data(), buf.size(), "", flags);
    buf.back() = 0;

    // NOTE: Mbed TLS writes each reason on its own line.
    std::string text;
    std::string_view rest = buf.data();
    while (!rest.empty()) {
        const auto end = rest.find('\n');
        if (const auto line = rest.substr(0, end); !line.empty()) {
            if (!text.empty()) {
                text += ", ";
            }
            text += line;
        }
        if (end == std::string_view::npos) {
            break;
        }
        rest.remove_prefix(end + 1);
    }
    return text;
}

#if defined(POMDOG_PLATFORM_LINUX)
/// Sends like mbedtls_net_send(), but a send to a closed connection fails
/// instead of raising SIGPIPE, which would end the process. Other platforms
/// keep mbedtls_net_send(): Winsock raises no signal, and the POSIX socket
/// helper sets SO_NOSIGPIPE on the platforms that have it.
[[nodiscard]] int
sendWithoutSignal(void* context, const unsigned char* buffer, std::size_t size)
{
    const auto descriptor = static_cast<::mbedtls_net_context*>(context)->fd;
    const auto result = ::send(descriptor, buffer, size, MSG_NOSIGNAL);
    if (result >= 0) {
        return static_cast<int>(result);
    }

    const auto errorCode = errors::toErrc(errno);
    if ((errorCode == std::errc::resource_unavailable_try_again) ||
        (errorCode == std::errc::operation_would_block) ||
        (errorCode == std::errc::interrupted)) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    }
    if ((errorCode == std::errc::broken_pipe) || (errorCode == std::errc::connection_reset)) {
        return MBEDTLS_ERR_NET_CONN_RESET;
    }
    return MBEDTLS_ERR_NET_SEND_FAILED;
}
#endif

/// Closes socket, a descriptor as mbedtls_net_context holds it, which the
/// connect worker got after the stream gave up on it.
void closeConnectedSocket(int socket) noexcept
{
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
    ::close(socket);
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    ::closesocket(static_cast<::SOCKET>(socket));
#endif
}

} // namespace

TLSStreamMbedTLS::TLSStreamMbedTLS() noexcept
{
    mbedtls_net_init(&descriptor_);
    mbedtls_ssl_init(&ssl_);
    mbedtls_ssl_config_init(&sslConfig_);
}

TLSStreamMbedTLS::~TLSStreamMbedTLS()
{
    // NOTE: A socket connection in progress does not hold up the destructor;
    // its thread closes the socket that it gets.
    connectWorker_.abandon();

    closeConnection();

    mbedtls_ssl_free(&ssl_);
    mbedtls_ssl_config_free(&sslConfig_);
}

std::expected<void, std::unique_ptr<Error>>
TLSStreamMbedTLS::connect(
    std::string_view host,
    std::string_view port,
    const StreamOptions& options,
    std::shared_ptr<TrustStoreMbedTLS>&& trustStore)
{
    POMDOG_ASSERT(trustStore != nullptr);

#if defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    if (auto result = winsock_.startup(); !result) {
        return std::unexpected(std::move(result.error()));
    }
#endif

    // NOTE: TLS uses the PSA random generator.
    if (auto result = initializePSACrypto(); !result) {
        return std::unexpected(std::move(result.error()));
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

    // NOTE: The handshake fails unless the server certificate verifies.
    trustStore_ = std::move(trustStore);
    mbedtls_ssl_conf_authmode(&sslConfig_, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&sslConfig_, trustStore_->getCertificates(), nullptr);

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
    auto errorContext = "couldn't connect to TLS socket on " + hostBuf + ":" + portBuf;
#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
    auto connectSocket = [hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = options.connectTimeout]() -> std::expected<int, std::unique_ptr<Error>> {
        auto result = connectSocketPOSIX(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);
        if (!result) {
            return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TLS socket on " + hostBuf + ":" + portBuf));
        }
        return *result;
    };
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
    auto connectSocket = [hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = options.connectTimeout, winsock = std::make_unique<WinsockLibrary>()]() -> std::expected<int, std::unique_ptr<Error>> {
        // NOTE: The thread keeps Winsock started on its own, since it may close
        // the socket after the stream is gone.
        if (auto started = winsock->startup(); !started) {
            return std::unexpected(std::move(started.error()));
        }
        auto result = connectSocketWin32(hostBuf, portBuf, SocketProtocol::TCP, connectTimeout);
        if (!result) {
            return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to TLS socket on " + hostBuf + ":" + portBuf));
        }
        return static_cast<int>(*result);
    };
#endif
    connectWorker_.start(std::move(connectSocket), closeConnectedSocket, options.connectTimeout, std::move(errorContext));

    state_ = State::Connecting;

    return {};
}

void TLSStreamMbedTLS::update()
{
    if (connectWorker_.isRunning()) {
        auto result = connectWorker_.poll();
        if (!result.has_value()) {
            return;
        }
        takeConnectedSocket(std::move(*result));
    }

    if (state_ == State::Handshaking) {
        handshake();
    }

    if (state_ == State::Connected) {
        flushSendBuffer();
        if (state_ != State::Connected) {
            return;
        }
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

void TLSStreamMbedTLS::takeConnectedSocket(std::expected<int, std::unique_ptr<Error>>&& result)
{
    // NOTE: close() gives up the worker, so a closed stream gets no socket.
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
#if defined(POMDOG_PLATFORM_LINUX)
    mbedtls_ssl_set_bio(&ssl_, &descriptor_, sendWithoutSignal, mbedtls_net_recv, nullptr);
#else
    mbedtls_ssl_set_bio(&ssl_, &descriptor_, mbedtls_net_send, mbedtls_net_recv, nullptr);
#endif

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

    if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        const auto flags = mbedtls_ssl_get_verify_result(&ssl_);
        disconnect(errors::make("the server certificate failed verification: " + verifyResultToString(flags)));
        return;
    }
    if (ret != 0) {
        disconnect(errors::make("mbedtls_ssl_handshake failed, " + mbedTLSErrorToString(ret)));
        return;
    }

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

    sendBuffer_.insert(std::end(sendBuffer_), std::begin(data), std::end(data));
    flushSendBuffer();
    return {};
}

void TLSStreamMbedTLS::flushSendBuffer()
{
    POMDOG_ASSERT(state_ == State::Connected);

    std::size_t sentSize = 0;
    while (sentSize < sendBuffer_.size()) {
        const auto rest = std::span{sendBuffer_}.subspan(sentSize);

        // NOTE: After MBEDTLS_ERR_SSL_WANT_WRITE, Mbed TLS keeps the record
        // that it built, and a later call sends it. That call reports the size
        // that it is given as written, so it must be given the same size.
        const auto size = (pendingWriteSize_ > 0) ? pendingWriteSize_ : rest.size();
        POMDOG_ASSERT(size <= rest.size());

        const auto ret = mbedtls_ssl_write(&ssl_, rest.data(), size);
        if (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
            // NOTE: A TLS 1.3 server may send session tickets after the
            // handshake. Mbed TLS reports each one before it writes.
            continue;
        }
        if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
            pendingWriteSize_ = size;
            break;
        }
        pendingWriteSize_ = 0;

        if (ret < 0) {
            disconnect(errors::make("mbedtls_ssl_write failed, " + mbedTLSErrorToString(ret)));
            return;
        }

        // NOTE: Mbed TLS writes one record per call, so it may write less than size.
        POMDOG_ASSERT(ret > 0);
        sentSize += static_cast<std::size_t>(ret);

        // NOTE: Update timestamp of last read/write
        lastActiveTime_ = Clock::now();
    }
    sendBuffer_.erase(std::begin(sendBuffer_), std::next(std::begin(sendBuffer_), static_cast<std::ptrdiff_t>(sentSize)));
}

void TLSStreamMbedTLS::close()
{
    connectWorker_.abandon();
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
    sendBuffer_.clear();
    pendingWriteSize_ = 0;
}

void TLSStreamMbedTLS::readSocket()
{
    POMDOG_ASSERT(state_ == State::Connected);

    if (receiveBuffer_.empty()) {
        receiveBuffer_.resize(receiveBufferSize);
    }

    // NOTE: Read until the connection has no more bytes, and report them as
    // one event before the stream closes. Each call returns at most one TLS
    // record, which may already wait inside Mbed TLS.
    std::vector<u8> data;
    std::unique_ptr<Error> err;
    bool isEndOfStream = false;
    while (data.size() < maxReceiveSizePerUpdate) {
        const auto size = std::min(receiveBuffer_.size(), maxReceiveSizePerUpdate - data.size());
        const auto ret = mbedtls_ssl_read(&ssl_, reinterpret_cast<unsigned char*>(receiveBuffer_.data()), size);

        if (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
            // NOTE: A TLS 1.3 server may send session tickets after the
            // handshake. Mbed TLS reports each one before application data.
            continue;
        }
        if ((ret == MBEDTLS_ERR_SSL_WANT_READ) || (ret == MBEDTLS_ERR_SSL_WANT_WRITE)) {
            break;
        }
        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            isEndOfStream = true;
            break;
        }
        if (ret == 0) {
            // NOTE: Zero means that the connection closed without close_notify,
            // as a man-in-the-middle can close it to cut the data short (RFC
            // 8446, section 6.1), so the stream reports an error.
            err = errors::make("the TLS connection closed without close_notify");
            break;
        }
        if (ret < 0) {
            err = errors::make("mbedtls_ssl_read failed, " + mbedTLSErrorToString(ret));
            break;
        }

        const auto readSize = static_cast<std::size_t>(ret);
        if (readSize > size) {
            err = errors::make("read size is larger than buffer size");
            break;
        }

        const auto view = std::span{receiveBuffer_}.first(readSize);
        data.insert(std::end(data), std::begin(view), std::end(view));
    }

    if (!data.empty()) {
        // NOTE: Update timestamp of last read/write
        lastActiveTime_ = Clock::now();
        events_.push_back(StreamEvent{.kind = StreamEventKind::Received, .data = std::move(data), .error = nullptr});
    }
    if ((err != nullptr) || isEndOfStream) {
        disconnect(std::move(err));
    }
}

} // namespace pomdog::network::detail
