// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "UDPStreamWin32.hpp"
#include "SocketHelperWin32.hpp"
#include "../Network/AddressParser.hpp"
#include "../Network/EndPoint.hpp"
#include "../Utility/ErrorHelper.hpp"
#include "Pomdog/Network/ArrayView.hpp"
#include "Pomdog/Network/IOService.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <WS2tcpip.h>
#include <array>
#include <cstring>

namespace Pomdog::Detail {
namespace {

bool isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

constexpr int flags = 0;

} // namespace

UDPStreamWin32::UDPStreamWin32(IOService* serviceIn)
    : service(serviceIn)
{
}

UDPStreamWin32::~UDPStreamWin32()
{
    if (blockingThread.joinable()) {
        blockingThread.join();
    }

    if (isSocketValid(this->descriptor)) {
        ::closesocket(this->descriptor);
        this->descriptor = INVALID_SOCKET;
    }
}

std::shared_ptr<Error>
UDPStreamWin32::Connect(const std::string_view& host, const std::string_view& port, const Duration& connectTimeout)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    std::thread connectThread([this, hostBuf = std::move(hostBuf), portBuf = std::move(portBuf), connectTimeout = connectTimeout] {
        auto [fd, err] = Detail::ConnectSocketWin32(hostBuf, portBuf, SocketProtocol::UDP, connectTimeout);

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
UDPStreamWin32::Listen(const std::string_view& host, const std::string_view& port)
{
    POMDOG_ASSERT(service != nullptr);

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto [fd, err] = Detail::BindSocketWin32(hostBuf, portBuf, SocketProtocol::UDP);

    if (err != nullptr) {
        auto wrapped = Errors::Wrap(std::move(err), "couldn't listen to UDP socket on " + hostBuf + ":" + portBuf);
        errorConn = service->ScheduleTask([this, err = std::move(wrapped)] {
            this->OnConnected(std::move(err));
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

void UDPStreamWin32::Close()
{
    this->eventLoopConn.Disconnect();
    this->errorConn.Disconnect();

    if (isSocketValid(this->descriptor)) {
        ::closesocket(this->descriptor);
        this->descriptor = INVALID_SOCKET;
    }
}

std::shared_ptr<Error>
UDPStreamWin32::Write(const ArrayView<std::uint8_t const>& data)
{
    POMDOG_ASSERT(isSocketValid(descriptor));
    POMDOG_ASSERT(data.GetData() != nullptr);
    POMDOG_ASSERT(data.GetSize() > 0);

    auto result = ::send(this->descriptor, reinterpret_cast<const char*>(data.GetData()), static_cast<int>(data.GetSize()), flags);

    if (result == SOCKET_ERROR) {
        return Errors::New("send failed with error: " + std::to_string(::WSAGetLastError()));
    }

    return nullptr;
}

std::shared_ptr<Error>
UDPStreamWin32::WriteTo(const ArrayView<std::uint8_t const>& data, const std::string_view& address)
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

    auto addrList = std::unique_ptr<struct ::addrinfo, void(WSAAPI*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<int> lastError;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        auto result = ::sendto(
            this->descriptor, reinterpret_cast<const char*>(data.GetData()), static_cast<int>(data.GetSize()), flags,
            info->ai_addr, static_cast<int>(info->ai_addrlen));

        if (result == SOCKET_ERROR) {
            lastError = ::WSAGetLastError();
            continue;
        }
        lastError = std::nullopt;
        break;
    }

    if (lastError != std::nullopt) {
        return Errors::New("sendto failed with error: " + std::to_string(*lastError));
    }

    return nullptr;
}

::SOCKET UDPStreamWin32::GetHandle() const noexcept
{
    return descriptor;
}

void UDPStreamWin32::ReadEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    const auto readSize = ::recv(this->descriptor, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags);
    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnRead({}, Errors::New("read failed with error: " + std::to_string(errorCode)));
        return;
    }

    // NOTE: When the peer socket has performed orderly shutdown,
    // the read size will be 0 (meaning the "end-of-file").
    POMDOG_ASSERT(readSize >= 0);
    auto view = ArrayView<std::uint8_t>{buffer.data(), static_cast<std::size_t>(readSize)};
    this->OnRead(std::move(view), nullptr);
}

void UDPStreamWin32::ReadFromEventLoop()
{
    POMDOG_ASSERT(isSocketValid(descriptor));

    // NOTE: Read per 1 frame (= 1/60 seconds) for a packet up to 1024 bytes.
    std::array<std::uint8_t, 1024> buffer;

    struct ::sockaddr_storage addrInfo;
    socklen_t addrLen = sizeof(addrInfo);

    const auto readSize = ::recvfrom(
        this->descriptor, reinterpret_cast<char*>(buffer.data()), static_cast<int>(buffer.size()), flags,
        reinterpret_cast<struct sockaddr*>(&addrInfo), &addrLen);

    if (readSize < 0) {
        const auto errorCode = ::WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK) {
            // NOTE: There is no data to be read yet
            return;
        }

        this->OnReadFrom({}, "", Errors::New("read failed with error: " + std::to_string(errorCode)));
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
