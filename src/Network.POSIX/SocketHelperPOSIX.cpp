// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "SocketHelperPOSIX.hpp"
#include "../Network/ErrorHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace Pomdog::Detail {
namespace {

struct timeval ToTimeval(const Duration& d)
{
    struct timeval tv;
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(d);
    const auto usec = d - std::chrono::duration_cast<Duration>(sec);
    tv.tv_sec = sec.count();
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>(std::chrono::duration_cast<std::chrono::microseconds>(usec).count());
    return tv;
}

} // namespace

std::tuple<int, std::shared_ptr<Error>>
ConnectSocketPOSIX(
    const std::string& host,
    const std::string& port,
    SocketProtocol protocol,
    const Duration& connectTimeoutIn)
{
    struct ::addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    switch (protocol) {
    case SocketProtocol::TCP:
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        break;
    case SocketProtocol::UDP:
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        break;
    default:
        break;
    }

    struct ::addrinfo* addrListRaw = nullptr;

    constexpr auto localhost = "127.0.0.1";
    auto hostName = host.data();
    if (host == "localhost") {
        hostName = localhost;
    }

    auto res = ::getaddrinfo(hostName, port.data(), &hints, &addrListRaw);
    if (res != 0) {
        return std::make_tuple(-1, Errors::New("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<std::errc> socketLastError;
    int descriptor = -1;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        // NOTE: Create a SOCKET for connecting to server
        descriptor = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (descriptor < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }

        // NOTE: Set non-blocking mode
        const int socketOpt = ::fcntl(descriptor, F_GETFL, 0);
        if (socketOpt < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }
        if (::fcntl(descriptor, F_SETFL, socketOpt | O_NONBLOCK) < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }

        // NOTE: Connect to server
        int result = ::connect(descriptor, info->ai_addr, static_cast<int>(info->ai_addrlen));
        if (result != 0) {
            socketLastError = Detail::ToErrc(errno);

            if (socketLastError == std::errc::operation_in_progress) {
                fd_set waitSet;

                FD_ZERO(&waitSet);
                FD_SET(descriptor, &waitSet);

                auto connectTimeout = ToTimeval(connectTimeoutIn);

                // NOTE: Wait for socket to be writable
                result = ::select(descriptor + 1, nullptr, &waitSet, nullptr, &connectTimeout);
            }
        }

        if (result < 0) {
            ::close(descriptor);
            descriptor = -1;
            continue;
        }

        socketLastError = std::nullopt;
        break;
    }

    if (socketLastError) {
        return std::make_tuple(-1, Errors::New(*socketLastError, "Unable to connect to server"));
    }

    return std::make_tuple(descriptor, nullptr);
}

std::tuple<int, std::shared_ptr<Error>>
BindSocketPOSIX(
    const std::string& host,
    const std::string& port,
    SocketProtocol protocol)
{
    struct ::addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    switch (protocol) {
    case SocketProtocol::TCP:
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        break;
    case SocketProtocol::UDP:
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        break;
    default:
        break;
    }
    hints.ai_flags = AI_PASSIVE;

    struct ::addrinfo* addrListRaw = nullptr;

    constexpr auto localhost = "127.0.0.1";
    auto hostName = host.data();
    if (host == "localhost") {
        hostName = localhost;
    }

    auto res = ::getaddrinfo(hostName, port.data(), &hints, &addrListRaw);
    if (res != 0) {
        return std::make_tuple(-1, Errors::New("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<std::errc> socketLastError;
    int descriptor = -1;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        // NOTE: Create a SOCKET for connecting to server
        descriptor = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (descriptor < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }

        // NOTE: Set non-blocking mode
        const int socketOpt = ::fcntl(descriptor, F_GETFL, 0);
        if (socketOpt < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }
        if (::fcntl(descriptor, F_SETFL, socketOpt | O_NONBLOCK) < 0) {
            socketLastError = Detail::ToErrc(errno);
            continue;
        }

        // NOTE: Setup the listening socket
        int result = ::bind(descriptor, info->ai_addr, static_cast<int>(info->ai_addrlen));
        if (result != 0) {
            socketLastError = Detail::ToErrc(errno);
            ::close(descriptor);
            descriptor = -1;
            continue;
        }

        socketLastError = std::nullopt;
        break;
    }

    if (socketLastError) {
        return std::make_tuple(-1, Errors::New(*socketLastError, "Unable to bind socket"));
    }

    return std::make_tuple(descriptor, nullptr);
}

} // namespace Pomdog::Detail
