// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/posix/socket_helper_posix.h"
#include "pomdog/network/connect_timeout.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/error_helper.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {
namespace {

using Clock = std::chrono::steady_clock;

/// Returns an error for errorCode, an errno value. It takes message as a
/// pointer, so that no allocation runs between reading errno and the call.
[[nodiscard]] std::unique_ptr<Error>
makeSocketError(int errorCode, const char* message) noexcept
{
    return errors::fromErrc(errors::toErrc(errorCode), std::string{message});
}

[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
setNonBlocking(int descriptor)
{
    const int flags = ::fcntl(descriptor, F_GETFL, 0);
    if (flags < 0) {
        return std::unexpected(makeSocketError(errno, "fcntl(F_GETFL) failed"));
    }
    if (::fcntl(descriptor, F_SETFL, flags | O_NONBLOCK) < 0) {
        return std::unexpected(makeSocketError(errno, "fcntl(F_SETFL) failed"));
    }
    return {};
}

/// Waits up to timeout for descriptor, which is connecting, to connect.
[[nodiscard]] std::expected<void, std::unique_ptr<Error>>
waitForConnection(int descriptor, Duration timeout)
{
    // NOTE: poll() has no limit on the descriptor's value, unlike select(),
    // whose fd_set holds only descriptors below FD_SETSIZE.
    const auto startTime = Clock::now();
    for (;;) {
        const auto timeLeft = timeout - std::chrono::duration_cast<Duration>(Clock::now() - startTime);
        if (timeLeft <= Duration::zero()) {
            return std::unexpected(errors::make("connect timed out"));
        }

        // NOTE: Round up, so that a wait shorter than a millisecond waits.
        const auto milliseconds = std::min(std::ceil(timeLeft.count() * 1000.0), static_cast<f64>(INT_MAX));

        struct ::pollfd target = {.fd = descriptor, .events = POLLOUT, .revents = 0};
        const int result = ::poll(&target, 1, static_cast<int>(milliseconds));
        if (result > 0) {
            break;
        }
        if ((result < 0) && (errno != EINTR)) {
            return std::unexpected(makeSocketError(errno, "poll failed"));
        }
    }

    // NOTE: The socket becomes writable both when the connection succeeds and
    // when it fails, and SO_ERROR tells which.
    int socketError = 0;
    socklen_t size = sizeof(socketError);
    if (::getsockopt(descriptor, SOL_SOCKET, SO_ERROR, &socketError, &size) < 0) {
        return std::unexpected(makeSocketError(errno, "getsockopt(SO_ERROR) failed"));
    }
    if (socketError != 0) {
        return std::unexpected(makeSocketError(socketError, "connect failed"));
    }
    return {};
}

/// Connects a new socket to the address in info, waiting up to timeout.
[[nodiscard]] std::expected<int, std::unique_ptr<Error>>
connectAddress(const struct ::addrinfo& info, Duration timeout)
{
    const int descriptor = ::socket(info.ai_family, info.ai_socktype, info.ai_protocol);
    if (descriptor < 0) {
        return std::unexpected(makeSocketError(errno, "socket failed"));
    }

    auto result = [&]() -> std::expected<void, std::unique_ptr<Error>> {
        if (auto nonBlocking = setNonBlocking(descriptor); !nonBlocking) {
            return std::unexpected(std::move(nonBlocking.error()));
        }

#if defined(SO_NOSIGPIPE)
        // NOTE: A send to a closed connection then fails with EPIPE instead
        // of raising SIGPIPE, which would end the process. Linux has no such
        // option, so the streams pass MSG_NOSIGNAL to each send there.
        int noSignal = 1;
        if (::setsockopt(descriptor, SOL_SOCKET, SO_NOSIGPIPE, &noSignal, sizeof(noSignal)) < 0) {
            return std::unexpected(makeSocketError(errno, "setsockopt(SO_NOSIGPIPE) failed"));
        }
#endif

        if (::connect(descriptor, info.ai_addr, info.ai_addrlen) == 0) {
            return {};
        }
        if (const int errorCode = errno; errorCode != EINPROGRESS) {
            return std::unexpected(makeSocketError(errorCode, "connect failed"));
        }
        return waitForConnection(descriptor, timeout);
    }();

    if (!result) {
        ::close(descriptor);
        return std::unexpected(std::move(result.error()));
    }
    return descriptor;
}

} // namespace

std::expected<int, std::unique_ptr<Error>>
connectSocketPOSIX(
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

    // NOTE: The deadline counts name resolution too.
    const auto startTime = Clock::now();

    auto res = ::getaddrinfo(hostName, port.data(), &hints, &addrListRaw);
    if (res != 0) {
        return std::unexpected(errors::make("getaddrinfo failed with error " + std::to_string(res)));
    }

    auto addrList = std::unique_ptr<struct ::addrinfo, void (*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::size_t addressCount = 0;
    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        ++addressCount;
    }

    // NOTE: Try the addresses in order, within one deadline for all of them,
    // and report the error of the last one when none connects.
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

std::expected<int, std::unique_ptr<Error>>
bindSocketPOSIX(
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

    auto addrList = std::unique_ptr<struct ::addrinfo, void (*)(struct ::addrinfo*)>{addrListRaw, ::freeaddrinfo};
    addrListRaw = nullptr;

    std::optional<std::errc> socketLastError;
    int descriptor = -1;

    for (auto info = addrList.get(); info != nullptr; info = info->ai_next) {
        // NOTE: Create a SOCKET for connecting to server
        descriptor = ::socket(info->ai_family, info->ai_socktype, info->ai_protocol);
        if (descriptor < 0) {
            socketLastError = errors::toErrc(errno);
            continue;
        }

        // NOTE: Set non-blocking mode
        const int socketOpt = ::fcntl(descriptor, F_GETFL, 0);
        if ((socketOpt < 0) || (::fcntl(descriptor, F_SETFL, socketOpt | O_NONBLOCK) < 0)) {
            socketLastError = errors::toErrc(errno);
            ::close(descriptor);
            descriptor = -1;
            continue;
        }

        // NOTE: Setup the listening socket
        int result = ::bind(descriptor, info->ai_addr, static_cast<int>(info->ai_addrlen));
        if (result != 0) {
            socketLastError = errors::toErrc(errno);
            ::close(descriptor);
            descriptor = -1;
            continue;
        }

        socketLastError = std::nullopt;
        break;
    }

    if (socketLastError) {
        return std::unexpected(errors::fromErrc(*socketLastError, "Unable to bind socket"));
    }

    return descriptor;
}

} // namespace pomdog::network::detail
