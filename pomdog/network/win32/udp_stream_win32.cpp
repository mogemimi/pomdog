// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/udp_stream_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/end_point.h"
#include "pomdog/network/host_port.h"
#include "pomdog/network/receive_limits.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WS2tcpip.h>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

[[nodiscard]] bool
isSocketValid(::SOCKET descriptor) noexcept
{
    return descriptor != INVALID_SOCKET;
}

constexpr int flags = 0;

} // namespace

UDPStreamWin32::UDPStreamWin32() noexcept = default;

UDPStreamWin32::~UDPStreamWin32()
{
    // NOTE: A connection in progress does not hold up the destructor; its
    // thread closes the socket that it gets.
    connectWorker_.abandon();
    closeSocket();
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamWin32::connect(std::string_view host, std::string_view port)
{
    if (auto result = winsock_.startup(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    // NOTE: A UDP stream has no options, so it gives up after five seconds.
    const auto connectTimeout = Duration{std::chrono::seconds{5}};
    connectWorker_.start(
        [host = std::string{host}, port = std::string{port}, connectTimeout, winsock = std::make_unique<WinsockLibrary>()]() -> std::expected<::SOCKET, std::unique_ptr<Error>> {
            // NOTE: The thread keeps Winsock started on its own, since it may
            // close the socket after the stream is gone.
            if (auto started = winsock->startup(); !started) {
                return std::unexpected(std::move(started.error()));
            }
            auto result = connectSocketWin32(host, port, SocketProtocol::UDP, connectTimeout);
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to UDP socket on " + host + ":" + port));
            }
            return result;
        },
        [](::SOCKET socket) noexcept { ::closesocket(socket); },
        connectTimeout,
        "couldn't connect to UDP socket on " + std::string{host} + ":" + std::string{port});

    return {};
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamWin32::listen(std::string_view host, std::string_view port)
{
    if (auto result = winsock_.startup(); !result) {
        return std::unexpected(std::move(result.error()));
    }

    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto result = bindSocketWin32(hostBuf, portBuf, SocketProtocol::UDP);
    if (!result) {
        return std::unexpected(errors::wrap(std::move(result.error()), "couldn't listen to UDP socket on " + hostBuf + ":" + portBuf));
    }
    descriptor_ = *result;
    isListening_ = true;

    return {};
}

void UDPStreamWin32::update()
{
    if (connectWorker_.isRunning()) {
        auto result = connectWorker_.poll();
        if (!result.has_value()) {
            return;
        }
        if (!*result) {
            disconnect(std::move(result->error()));
            return;
        }
        descriptor_ = **result;
    }

    if (isClosed_ || !isSocketValid(descriptor_)) {
        return;
    }

    if (!isConnectedQueued_) {
        isConnectedQueued_ = true;
        events_.push_back(UDPStreamEvent{.kind = StreamEventKind::Connected, .data = {}, .address = {}, .error = nullptr});
    }

    readSocket();
}

std::optional<UDPStreamEvent>
UDPStreamWin32::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamWin32::write(std::span<const u8> data)
{
    if (isClosed_ || !isSocketValid(descriptor_)) {
        return std::unexpected(errors::make("the UDP stream is not connected"));
    }

    auto result = ::send(descriptor_, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), flags);

    if (result == SOCKET_ERROR) {
        return std::unexpected(errors::make("send failed with error: " + std::to_string(::WSAGetLastError())));
    }

    return {};
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamWin32::writeTo(std::span<const u8> data, std::string_view address)
{
    if (isClosed_ || !isSocketValid(descriptor_)) {
        return std::unexpected(errors::make("the UDP stream is not connected"));
    }

    const auto [hostView, portView] = splitHostPort(address);
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
        return std::unexpected(errors::make("getaddrinfo failed with error " + std::to_string(res)));
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
        return std::unexpected(errors::make("sendto failed with error: " + std::to_string(*lastError)));
    }

    return {};
}

void UDPStreamWin32::close()
{
    isClosed_ = true;
    connectWorker_.abandon();
    closeSocket();
    events_.clear();
}

void UDPStreamWin32::disconnect(std::unique_ptr<Error>&& err)
{
    isClosed_ = true;
    closeSocket();
    events_.push_back(UDPStreamEvent{.kind = StreamEventKind::Disconnected, .data = {}, .address = {}, .error = std::move(err)});
}

void UDPStreamWin32::closeSocket() noexcept
{
    if (isSocketValid(descriptor_)) {
        ::closesocket(descriptor_);
        descriptor_ = INVALID_SOCKET;
    }
}

void UDPStreamWin32::readSocket()
{
    POMDOG_ASSERT(isSocketValid(descriptor_));

    if (receiveBuffer_.empty()) {
        receiveBuffer_.resize(receiveBufferSize);
    }

    // NOTE: Read datagrams until the socket has no more. Each read counts as
    // at least a byte, so that a flood of empty datagrams also ends the loop.
    std::size_t receivedSize = 0;
    while (receivedSize < maxReceiveSizePerUpdate) {
        struct ::sockaddr_storage addrInfo;
        int addrLen = sizeof(addrInfo);

        const auto buffer = reinterpret_cast<char*>(receiveBuffer_.data());
        const auto bufferSize = static_cast<int>(receiveBuffer_.size());
        const auto readSize = isListening_
                                  ? ::recvfrom(descriptor_, buffer, bufferSize, flags, reinterpret_cast<struct sockaddr*>(&addrInfo), &addrLen)
                                  : ::recv(descriptor_, buffer, bufferSize, flags);

        if (readSize == SOCKET_ERROR) {
            const auto errorCode = ::WSAGetLastError();
            if (errorCode == WSAEWOULDBLOCK) {
                // NOTE: There is no data to be read yet
                return;
            }
            if (errorCode == WSAECONNRESET) {
                // NOTE: An ICMP "port unreachable" message for an earlier datagram
                // reports a peer that did not receive it, not a failure of this
                // socket, so the stream keeps receiving.
                receivedSize += 1;
                continue;
            }

            disconnect(errors::make("read failed with error: " + std::to_string(errorCode)));
            return;
        }

        POMDOG_ASSERT(readSize >= 0);
        const auto view = std::span{receiveBuffer_}.first(static_cast<std::size_t>(readSize));
        receivedSize += std::max<std::size_t>(view.size(), 1);

        UDPStreamEvent event = {
            .kind = StreamEventKind::Received,
            .data = std::vector<u8>(view.begin(), view.end()),
            .address = {},
            .error = nullptr,
        };
        if (isListening_) {
            event.address = EndPoint::createFromAddressStorage(addrInfo).toString();
        }
        events_.push_back(std::move(event));
    }
}

} // namespace pomdog::network::detail
