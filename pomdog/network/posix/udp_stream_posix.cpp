// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/udp_stream_posix.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/network/address_parser.h"
#include "pomdog/network/end_point.h"
#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/network/receive_limits.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
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

UDPStreamPOSIX::UDPStreamPOSIX() noexcept = default;

UDPStreamPOSIX::~UDPStreamPOSIX()
{
    // NOTE: Wait for a connection in progress, and close its socket.
    if (connectResult_.valid()) {
        if (auto result = connectResult_.get(); result) {
            ::close(*result);
        }
    }
    closeSocket();
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamPOSIX::connect(std::string_view host, std::string_view port)
{
    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    connectResult_ = std::async(
        std::launch::async,
        [host = std::string{host}, port = std::string{port}]() -> std::expected<int, std::unique_ptr<Error>> {
            auto result = connectSocketPOSIX(host, port, SocketProtocol::UDP, std::chrono::seconds{5});
            if (!result) {
                return std::unexpected(errors::wrap(std::move(result.error()), "couldn't connect to UDP socket on " + host + ":" + port));
            }
            return result;
        });

    return {};
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamPOSIX::listen(std::string_view host, std::string_view port)
{
    // NOTE: A std::string_view doesn't provide a conversion to a const char* because it doesn't store a null-terminated string.
    const auto hostBuf = std::string{host};
    const auto portBuf = std::string{port};

    auto result = bindSocketPOSIX(hostBuf, portBuf, SocketProtocol::UDP);
    if (!result) {
        return std::unexpected(errors::wrap(std::move(result.error()), "couldn't listen to UDP socket on " + hostBuf + ":" + portBuf));
    }
    descriptor_ = *result;
    isListening_ = true;

    return {};
}

void UDPStreamPOSIX::update()
{
    if (connectResult_.valid()) {
        if (connectResult_.wait_for(std::chrono::seconds{0}) != std::future_status::ready) {
            return;
        }

        auto result = connectResult_.get();
        if (isClosed_) {
            // NOTE: The stream was closed while connecting.
            if (result) {
                ::close(*result);
            }
            return;
        }
        if (!result) {
            disconnect(std::move(result.error()));
            return;
        }
        descriptor_ = *result;
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
UDPStreamPOSIX::pollEvent()
{
    if (events_.empty()) {
        return std::nullopt;
    }
    auto event = std::move(events_.front());
    events_.pop_front();
    return event;
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamPOSIX::write(std::span<const u8> data)
{
    if (isClosed_ || !isSocketValid(descriptor_)) {
        return std::unexpected(errors::make("the UDP stream is not connected"));
    }

    auto result = ::send(descriptor_, data.data(), data.size(), flags);

    if (result == -1) {
        const auto errorCode = errors::toErrc(errno);
        return std::unexpected(errors::fromErrc(errorCode, "send failed with error"));
    }

    return {};
}

std::expected<void, std::unique_ptr<Error>>
UDPStreamPOSIX::writeTo(std::span<const u8> data, std::string_view address)
{
    if (isClosed_ || !isSocketValid(descriptor_)) {
        return std::unexpected(errors::make("the UDP stream is not connected"));
    }

    const auto [family, hostView, portView] = AddressParser::transformAddress(address);
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
            lastError = errors::toErrc(errno);
            continue;
        }
        lastError = std::nullopt;
        break;
    }

    if (lastError != std::nullopt) {
        return std::unexpected(errors::fromErrc(*lastError, "sendto failed with error"));
    }

    return {};
}

void UDPStreamPOSIX::close()
{
    isClosed_ = true;
    closeSocket();
    events_.clear();
}

void UDPStreamPOSIX::disconnect(std::unique_ptr<Error>&& err)
{
    isClosed_ = true;
    closeSocket();
    events_.push_back(UDPStreamEvent{.kind = StreamEventKind::Disconnected, .data = {}, .address = {}, .error = std::move(err)});
}

void UDPStreamPOSIX::closeSocket() noexcept
{
    if (isSocketValid(descriptor_)) {
        ::close(descriptor_);
        descriptor_ = InvalidSocket;
    }
}

void UDPStreamPOSIX::readSocket()
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
        socklen_t addrLen = sizeof(addrInfo);

        const auto readSize = isListening_
                                  ? ::recvfrom(
                                        descriptor_, receiveBuffer_.data(), receiveBuffer_.size(), flags,
                                        reinterpret_cast<struct sockaddr*>(&addrInfo), &addrLen)
                                  : ::recv(descriptor_, receiveBuffer_.data(), receiveBuffer_.size(), flags);

        if (readSize < 0) {
            const auto errorCode = errors::toErrc(errno);
            if (errorCode == std::errc::resource_unavailable_try_again || errorCode == std::errc::operation_would_block) {
                // NOTE: There is no data to be read yet
                return;
            }
            if (errorCode == std::errc::connection_refused) {
                // NOTE: An ICMP "port unreachable" message for an earlier datagram
                // reports a peer that did not receive it, not a failure of this
                // socket, so the stream keeps receiving.
                receivedSize += 1;
                continue;
            }

            disconnect(errors::fromErrc(errorCode, "read failed with error"));
            return;
        }

        if (static_cast<std::size_t>(readSize) > receiveBuffer_.size()) {
            disconnect(errors::make("read size is larger than buffer size"));
            return;
        }

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
