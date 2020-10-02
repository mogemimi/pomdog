// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "UDPStreamPOSIX.hpp"
#include "SocketHelperPOSIX.hpp"
#include "../Network/AddressParser.hpp"
#include "../Network/EndPoint.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <array>
#include <cstring>

namespace Pomdog::Detail {
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
    : service(serviceIn)
{
}

UDPStreamPOSIX::~UDPStreamPOSIX()
{
    if (blockingThread.joinable()) {
        blockingThread.join();
    }

    if (isSocketValid(this->descriptor)) {
        ::close(this->descriptor);
        this->descriptor = InvalidSocket;
    }
}

std::shared_ptr<Error>
UDPStreamPOSIX::Connect(std::string_view host, std::string_view port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = Detail::ConnectSocketPOSIX(hostBuf, portBuf, SocketProtocol::UDP, connectTimeout);

        if (err != nullptr) {
            auto wrapped = Errors::Wrap(std::move(err), "couldn't connect to UDP socket on " + hostBuf + ":" + portBuf);
            errorConn = service->ScheduleTask([this, err = std::move(wrapped)] {
                this->OnConnected(std::move(err));
                this->errorConn.Disconnect();
            });
            return;
        }
        this->descriptor = fd;

        eventLoopConn = service->ScheduleTask([this] {
            this->OnConnected(nullptr);
            eventLoopConn.Disconnect();
            eventLoopConn = service->ScheduleTask([this] { this->ReadEventLoop(); });
        });
    });

    this->blockingThread = std::move(connectThread);

    return nullptr;
}

std::shared_ptr<Error>
UDPStreamPOSIX::Listen(std::string_view host, std::string_view port)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto [fd, err] = Detail::BindSocketPOSIX(hostBuf, portBuf, SocketProtocol::UDP);

    if (err != nullptr) {
        errorConn = service->ScheduleTask([this, err = std::move(err)] {
            this->OnConnected(err);
            this->errorConn.Disconnect();
        });
        return err;
    }
    this->descriptor = fd;

    eventLoopConn = service->ScheduleTask([this] {
        this->OnConnected(nullptr);
        eventLoopConn.Disconnect();
        eventLoopConn = service->ScheduleTask([this] { this->ReadFromEventLoop(); });
    });

    return nullptr;
}

void UDPStreamPOSIX::Close()
{
    this->eventLoopConn.Disconnect();
    this->errorConn.Disconnect();

    if (isSocketValid(this->descriptor)) {
        ::close(this->descriptor);
        this->descriptor = InvalidSocket;
    }
}

std::shared_ptr<Error>
UDPStreamPOSIX::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor));
    POMDOG_ASSERT(data.GetData() != nullptr);
    POMDOG_ASSERT(data.GetSize() > 0);

    auto result = ::send(this->descriptor, data.GetData(), data.GetSize(), flags);

    if (result == -1) {
        auto errorCode = Detail::ToErrc(errno);
        return Errors::New(errorCode, "send failed with error");
    }

    return nullptr;
}

std::shared_ptr<Error>
UDPStreamPOSIX::WriteTo(const ArrayView<std::uint8_t const>& data, std::string_view address)
{
    POMDOG_ASSERT(isSocketValid(this->descriptor));
    POMDOG_ASSERT(data.GetData() != nullptr);
    POMDOG_ASSERT(data.GetSize() > 0);

    const auto [family, hostView, portView] = Detail::AddressParser::TransformAddress(address);
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
        return Errors::New("getaddrinfo failed with error " + std::to_string(res));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<std::errc> lastError;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        auto result = ::sendto(
            this->descriptor, data.GetData(), data.GetSize(), flags,
            info->ai_addr, static_cast<int>(info->ai_addrlen));

        if (result == -1) {
            lastError = Detail::ToErrc(errno);
            continue;
        }
        lastError = std::nullopt;
        break;
    }

    if (lastError != std::nullopt) {
        return Errors::New(*lastError, "sendto failed with error");
    }

    return nullptr;
}

int UDPStreamPOSIX::GetHandle() const noexcept
{
    return descriptor;
}

void UDPStreamPOSIX::ReadEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    const auto readSize = ::recv(this->descriptor, buffer.data(), buffer.size(), flags);
    if (readSize == -1) {
        const auto errorCode = Detail::ToErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnRead({}, Errors::New(errorCode, "read failed with error"));
        return;
    }

    if (readSize < 0) {
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    this->OnRead(std::move(view), nullptr);
}

void UDPStreamPOSIX::ReadFromEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    struct ::sockaddr_storage addrInfo;
    socklen_t addrLen = sizeof(addrInfo);

    const auto readSize = ::recvfrom(
        this->descriptor, buffer.data(), buffer.size(), flags,
        reinterpret_cast<struct sockaddr*>(&addrInfo), &addrLen);

    if (readSize == -1) {
        const auto errorCode = Detail::ToErrc(errno);
        if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnReadFrom({}, "", Errors::New(errorCode, "read failed with error"));
        return;
    }

    if (readSize < 0) {
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto addr = EndPoint::CreateFromAddressStorage(addrInfo);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    this->OnReadFrom(std::move(view), addr.ToString(), nullptr);
}

} // namespace Pomdog::Detail
