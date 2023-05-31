// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/udp_stream_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/address_parser.h"
#include "pomdog/network/array_view.h"
#include "pomdog/network/end_point.h"
#include "pomdog/network/io_service.h"
#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WS2tcpip.h>
#include <array>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

[[nodiscard]] bool
isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

constexpr int flags = 0;

} // namespace

UDPStreamWin32::UDPStreamWin32(IOService* serviceIn)
    : service_(serviceIn)
{
}

UDPStreamWin32::~UDPStreamWin32()
{
    if (blockingThread_.joinable()) {
        blockingThread_.join();
    }

    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

std::unique_ptr<Error>
UDPStreamWin32::connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = detail::connectSocketWin32(hostBuf, portBuf, SocketProtocol::UDP, connectTimeout);

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
UDPStreamWin32::listen(std::string_view host, std::string_view port)
{
    POMDOG_ASSERT(service_ != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto [fd, err] = detail::bindSocketWin32(hostBuf, portBuf, SocketProtocol::UDP);

    if (err != nullptr) {
        auto wrapped = errors::wrap(std::move(err), "couldn't listen to UDP socket on " + hostBuf + ":" + portBuf);
        std::shared_ptr<Error> shared = wrapped->clone();
        errorConn_ = service_->scheduleTask([this, err = std::move(shared)] {
            onConnected(err->clone());
            errorConn_.disconnect();
        });
        return wrapped;
    }
    descriptor_ = fd;

    eventLoopConn_ = service_->scheduleTask([this] {
        onConnected(nullptr);
        eventLoopConn_.disconnect();
        eventLoopConn_ = service_->scheduleTask([this] { readFromEventLoop(); });
    });

    return nullptr;
}

void UDPStreamWin32::close()
{
    eventLoopConn_.disconnect();
    errorConn_.disconnect();

    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

std::unique_ptr<Error>
UDPStreamWin32::write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(data.size() > 0);

    auto result = ::send(descriptor_, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), flags);

    if (result == SOCKET_ERROR) {
        return errors::make("send failed with error: " + std::to_string(::WSAGetLastError()));
    }

    return nullptr;
}

std::unique_ptr<Error>
UDPStreamWin32::writeTo(const ArrayView<std::uint8_t const>& data, std::string_view address)
{
    POMDOG_ASSERT(isSocketValid(descriptor_));
    POMDOG_ASSERT(data.data() != nullptr);
    POMDOG_ASSERT(data.size() > 0);

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

    auto addrList = std::unique_ptr<struct ::addrinfo, void(WSAAPI*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<int> lastError;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        auto result = ::sendto(
            descriptor_,
            reinterpret_cast<const char*>(data.data()),
            static_cast<int>(data.size()),
            flags,
            info->ai_addr,
            static_cast<int>(info->ai_addrlen));

        if (result == SOCKET_ERROR) {
            lastError = ::WSAGetLastError();
            continue;
        }
        lastError = std::nullopt;
        break;
    }

    if (lastError != std::nullopt) {
        return errors::make("sendto failed with error: " + std::to_string(*lastError));
    }

    return nullptr;
}

::SOCKET
UDPStreamWin32::getHandle() const noexcept
{
    return descriptor_;
}

void UDPStreamWin32::readEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    const auto readSize = ::recv(descriptor_, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags);
    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        onRead({}, errors::make("read failed with error: " + std::to_string(errorCode)));
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    onRead(std::move(view), nullptr);
}

void UDPStreamWin32::readFromEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    struct ::sockaddr_storage addrInfo;
    socklen_t addrLen = sizeof(addrInfo);

    const auto readSize = ::recvfrom(
        descriptor_, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags,
        reinterpret_cast<struct sockaddr*>(&addrInfo), &addrLen);

    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        onReadFrom({}, "", errors::make("read failed with error: " + std::to_string(errorCode)));
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
