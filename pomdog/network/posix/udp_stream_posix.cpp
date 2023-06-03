// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/udp_stream_posix.h"
#include "pomdog/network/address_parser.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/end_point.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

constexpr int InvalidSocket = -1;

bool isSocketValid(int descriptor) noexcept
{
    return descriptor >= 0;
}

#if defined(POMDOG_PLATFORM_LINUX)
constexpr int flags = MSG_NOSIGNAL;
#else
constexpr int flags = 0;
#endif

} // namespace

UDPStreamPOSIX::UDPStreamPOSIX(IOService* serviceIn)
    : service_(serviceIn)
{
}

UDPStreamPOSIX::~UDPStreamPOSIX()
{
    if (blockingThread_.joinable()) {
        blockingThread_.join();
    }

    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

std::unique_ptr<Error>
UDPStreamPOSIX::connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = detail::connectSocketPOSIX(hostBuf, portBuf, SocketProtocol::UDP, connectTimeout);

        if (err != nullptr) {
            auto wrapped = errors::wrap(std::move(err), "couldn't connect to UDP socket on " + hostBuf + ":" + portBuf);
            std::shared_ptr<Error> shared = std::move(wrapped);
            errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
                onConnected(err->clone());
                errorConn_.disconnect();
            });
            return;
        }
        descriptor_ = fd;

        eventLoopConn_ = service_->scheduleTask([this] {
            onConnected(nullptr);
            eventLoopConn_.disconnect();
            eventLoopConn_ = service_->scheduleTask([this] { readEventLoop(); });
        });
    });

    blockingThread_ = std::move(connectThread);

    return nullptr;
}

std::unique_ptr<Error>
UDPStreamPOSIX::listen(std::string_view host, std::string_view port)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto [fd, err] = detail::bindSocketPOSIX(hostBuf, portBuf, SocketProtocol::UDP);

    if (err != nullptr) {
        std::shared_ptr<Error> shared = err->clone();
        errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
            onConnected(err->clone());
            errorConn_.disconnect();
        });
        return std::move(err);
    }
    descriptor_ = fd;

    eventLoopConn_ = service_->scheduleTask([this] {
        onConnected(nullptr);
        eventLoopConn_.disconnect();
        eventLoopConn_ = service_->scheduleTask([this] { readFromEventLoop(); });
    });

    return nullptr;
}

void UDPStreamPOSIX::close()
{
    eventLoopConn_.disconnect();
    errorConn_.disconnect();

    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

std::unique_ptr<Error>
UDPStreamPOSIX::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(!data.empty());

    auto result = ::send(descriptor_, data.data(), data.size(), flags);

    if (result == -1) {
        auto errorCode = detail::ToErrc(errno);
        return errors::makeIOError(errorCode, "send failed with error");
    }

    return nullptr;
}

std::unique_ptr<Error>
UDPStreamPOSIX::writeTo(const ArrayView<std::uint8_t const>& data, std::string_view address)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(!data.empty());

    const auto [family, hostView, portView] = detail::AddressParser::transformAddress(address);
    auto host = std::string{hostView};
    auto port = std::string{portView};

    struct ::addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct ::addrinfo* addrListRaw = nullptr;

    auto res = ::getaddrinfo(host.data(), port.data(), &hints, &addrListRaw);
    if (res != 0) {
        return errors::make("getaddrinfo failed with error " + std::to_string(res));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void (*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<std::errc> lastError;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        auto result = ::sendto(
            descriptor_,
            data.data(),
            data.size(),
            flags,
            info->ai_addr,
            static_cast<int>(info->ai_addrlen));

        if (result == -1) {
            lastError = detail::ToErrc(errno);
            continue;
        }
        lastError = std::nullopt;
        break;
    }

    if (lastError != std::nullopt) {
        return errors::makeIOError(*lastError, "sendto failed with error");
    }

    return nullptr;
}

int UDPStreamPOSIX::getHandle() const noexcept
{
    return descriptor_;
}

void UDPStreamPOSIX::readEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    const auto readSize = ::recv(descriptor_, buffer.data(), buffer.size(), flags);
    if (readSize == -1) {
        const auto errorCode = detail::ToErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        onRead({}, errors::makeIOError(errorCode, "read failed with error"));
        return;
    }

    if (readSize < 0) {
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    onRead(std::move(view), nullptr);
}

void UDPStreamPOSIX::readFromEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    struct ::sockaddr_storage addrInfo;
    socklen_t addrLen = sizeof(addrInfo);

    const auto readSize = ::recvfrom(
        descriptor_,
        buffer.data(),
        buffer.size(),
        flags,
        reinterpret_cast<struct sockaddr*>(&addrInfo),
        &addrLen);

    if (readSize == -1) {
        const auto errorCode = detail::ToErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        onReadFrom({}, "", errors::makeIOError(errorCode, "read failed with error"));
        return;
    }

    if (readSize < 0) {
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto addr = EndPoint::createFromAddressStorage(addrInfo);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    onReadFrom(std::move(view), addr.toString(), nullptr);
}

} // namespace pomdog::detail
