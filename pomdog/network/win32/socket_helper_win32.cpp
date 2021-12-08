// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/socket_helper_win32.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/error_helper.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WS2tcpip.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {
namespace {

struct timeval ToTimeval(const Duration& d)
{
    struct timeval tv;
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(d);
    const auto usec = d - std::chrono::duration_cast<Duration>(sec);
    tv.tv_sec = static_cast<decltype(tv.tv_sec)>(sec.count());
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>(std::chrono::duration_cast<std::chrono::microseconds>(usec).count());
    return tv;
}

} // namespace

std::tuple<::SOCKET, std::unique_ptr<Error>>
ConnectSocketWin32(
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
        return std::make_tuple(INVALID_SOCKET, errors::New("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(WSAAPI*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<int> socketLastError;
    ::SOCKET descriptor = INVALID_SOCKET;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        // NOTE: Create a SOCKET for connecting to server
        descriptor = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (descriptor == INVALID_SOCKET) {
            socketLastError = ::WSAGetLastError();
            continue;
        }

        // NOTE: Set non-blocking mode
        {
            ::u_long nonBlocking = 1;
            auto ret = ::ioctlsocket(descriptor, FIONBIO, &nonBlocking);

            if (ret != 0) {
                socketLastError = ::WSAGetLastError();
                continue;
            }
        }

        // NOTE: Connect to server
        int result = ::connect(descriptor, info->ai_addr, static_cast<int>(info->ai_addrlen));
        if (result != 0) {
            socketLastError = ::WSAGetLastError();

            if (socketLastError == WSAEWOULDBLOCK) {
                fd_set waitSet;

                FD_ZERO(&waitSet);
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4548)
#endif
                FD_SET(descriptor, &waitSet);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

                auto connectTimeout = ToTimeval(connectTimeoutIn);

                // NOTE: Wait for socket to be writable
                result = ::select(0, nullptr, &waitSet, nullptr, &connectTimeout);
            }
        }

        if (result < 0) {
            ::closesocket(descriptor);
            descriptor = INVALID_SOCKET;
            continue;
        }

        socketLastError = std::nullopt;
        break;
    }

    if (socketLastError) {
        return std::make_tuple(INVALID_SOCKET, errors::New("Unable to connect to server " + std::to_string(*socketLastError)));
    }

    return std::make_tuple(descriptor, nullptr);
}

std::tuple<::SOCKET, std::unique_ptr<Error>>
BindSocketWin32(
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
        return std::make_tuple(INVALID_SOCKET, errors::New("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(WSAAPI*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<int> socketLastError;
    ::SOCKET descriptor = INVALID_SOCKET;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        // NOTE: Create a SOCKET for connecting to server
        descriptor = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (descriptor == INVALID_SOCKET) {
            socketLastError = ::WSAGetLastError();
            continue;
        }

        // NOTE: Set non-blocking mode
        {
            ::u_long nonBlocking = 1;
            auto ret = ::ioctlsocket(descriptor, FIONBIO, &nonBlocking);

            if (ret != 0) {
                socketLastError = ::WSAGetLastError();
                continue;
            }
        }

        // NOTE: Setup the listening socket
        int result = ::bind(descriptor, info->ai_addr, static_cast<int>(info->ai_addrlen));
        if (result != 0) {
            socketLastError = ::WSAGetLastError();
            ::closesocket(descriptor);
            descriptor = INVALID_SOCKET;
            continue;
        }

        socketLastError = std::nullopt;
        break;
    }

    if (socketLastError) {
        return std::make_tuple(INVALID_SOCKET, errors::New("Unable to bind socket " + std::to_string(*socketLastError)));
    }

    return std::make_tuple(descriptor, nullptr);
}

} // namespace pomdog::detail
