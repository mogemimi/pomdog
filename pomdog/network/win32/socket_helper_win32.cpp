// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/socket_helper_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/network/connect_timeout.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WS2tcpip.h>
#include <algorithm>
#include <chrono>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

[[nodiscard]] struct timeval
toTimeval(const Duration& d) noexcept
{
    struct timeval tv;
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(d);
    const auto usec = d - std::chrono::duration_cast<Duration>(sec);
    tv.tv_sec = static_cast<decltype(tv.tv_sec)>(sec.count());
    tv.tv_usec = static_cast<decltype(tv.tv_usec)>(std::chrono::duration_cast<std::chrono::microseconds>(usec).count());
    return tv;
}

[[nodiscard]] std::unique_ptr<Error>
makeSocketError(const char* function, int errorCode) noexcept
{
    return errors::make(std::string{function} + " failed with error " + std::to_string(errorCode));
}

/// Waits up to timeout for descriptor, which is connecting, to connect.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
waitForConnection(::SOCKET descriptor, Duration timeout)
{
    const auto startTime = Clock::now();
    for (;;) {
        const auto timeLeft = timeout - std::chrono::duration_cast<Duration>(Clock::now() - startTime);
        if (timeLeft <= Duration::zero()) {
            return std::unexpected(errors::make("connect timed out"));
        }

        // NOTE: Winsock reports a connection that succeeded in the write set,
        // and one that failed in the except set.
        fd_set writeSet;
        fd_set exceptSet;
        FD_ZERO(&writeSet);
        FD_ZERO(&exceptSet);
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4548)
#endif
        FD_SET(descriptor, &writeSet);
        FD_SET(descriptor, &exceptSet);
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

        // NOTE: Wait an hour at most per call, so that the seconds fit in a long.
        auto waitTime = toTimeval(std::min(timeLeft, Duration{std::chrono::hours{1}}));

        const int result = ::select(0, nullptr, &writeSet, &exceptSet, &waitTime);
        if (result == SOCKET_ERROR) {
            return std::unexpected(makeSocketError("select", ::WSAGetLastError()));
        }
        if (result == 0) {
            continue;
        }

        if (FD_ISSET(descriptor, &exceptSet)) {
            int socketError = 0;
            int size = sizeof(socketError);
            if (::getsockopt(descriptor, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&socketError), &size) != 0) {
                socketError = ::WSAGetLastError();
            }
            return std::unexpected(makeSocketError("connect", socketError));
        }
        return {};
    }
}

/// Connects a new socket to the address in info, waiting up to timeout.
[[nodiscard]] std::expected<::SOCKET, std::unique_ptr<Error>>
connectAddress(const struct ::addrinfo& info, Duration timeout)
{
    const ::SOCKET descriptor = ::socket(info.ai_family, info.ai_socktype, info.ai_protocol);
    if (descriptor == INVALID_SOCKET) {
        return std::unexpected(makeSocketError("socket", ::WSAGetLastError()));
    }

    auto result = [&]() -> std::expected<void, std::unique_ptr<Error>> {
        ::u_long nonBlocking = 1;
        if (::ioctlsocket(descriptor, FIONBIO, &nonBlocking) != 0) {
            return std::unexpected(makeSocketError("ioctlsocket", ::WSAGetLastError()));
        }

        if (::connect(descriptor, info.ai_addr, static_cast<int>(info.ai_addrlen)) == 0) {
            return {};
        }
        if (const int errorCode = ::WSAGetLastError(); errorCode != WSAEWOULDBLOCK) {
            return std::unexpected(makeSocketError("connect", errorCode));
        }
        return waitForConnection(descriptor, timeout);
    }();

    if (!result) {
        ::closesocket(descriptor);
        return std::unexpected(std::move(result.error()));
    }
    return descriptor;
}

} // namespace

[[nodiscard]] std::expected<::SOCKET, std::unique_ptr<Error>>
connectSocketWin32(
    const std::string& host,
    const std::string& port,
    SocketProtocol protocol,
    const Duration& connectTimeout)
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
    }

    struct ::addrinfo* addrListRaw = nullptr;

    constexpr auto localhost = "127.0.0.1";
    auto hostName = host.data();
    if (host == "localhost") {
        hostName = localhost;
    }

    auto res = ::getaddrinfo(hostName, port.data(), &hints, &addrListRaw);
    if (res != 0) {
        return std::unexpected(errors::make("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void(WSAAPI*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::size_t addressCount = 0;
    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        ++addressCount;
    }

    // NOTE: Try the addresses in order, within one deadline for all of them,
    // and report the error of the last one when none connects.
    const auto startTime = Clock::now();
    std::unique_ptr<Error> lastError;
    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        const auto timeLeft = connectTimeout - std::chrono::duration_cast<Duration>(Clock::now() - startTime);
        if (timeLeft <= Duration::zero()) {
            lastError = errors::make("connect timed out");
            break;
        }

        auto descriptor = connectAddress(*info, splitConnectTimeout(timeLeft, addressCount));
        if (descriptor) {
            return *descriptor;
        }
        lastError = std::move(descriptor.error());
        --addressCount;
    }

    if (lastError == nullptr) {
        lastError = errors::make("getaddrinfo returned no address");
    }
    return std::unexpected(std::move(lastError));
}

[[nodiscard]] std::expected<::SOCKET, std::unique_ptr<Error>>
bindSocketWin32(
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
        return std::unexpected(errors::make("getaddrinfo failed with error " + std::to_string(res)));
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
                ::closesocket(descriptor);
                descriptor = INVALID_SOCKET;
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
        return std::unexpected(errors::make("Unable to bind socket " + std::to_string(*socketLastError)));
    }

    return descriptor;
}

} // namespace pomdog::network::detail
