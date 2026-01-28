// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/end_point.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstring>
#include <sstream>

#if defined(POMDOG_PLATFORM_MACOSX) ||    \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include <arpa/inet.h>
#include <netdb.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

EndPoint::EndPoint()
{
    std::memset(&address_, 0, sizeof(address_));
    address_.asV6.sin6_family = AF_INET6;
}

AddressFamily EndPoint::getFamily() const noexcept
{
    if (address_.asV6.sin6_family == AF_INET6) {
        POMDOG_ASSERT(address_.asV4.sin_family == AF_INET6);
        return AddressFamily::InterNetworkV6;
    }
    POMDOG_ASSERT(address_.asV4.sin_family == AF_INET);
    POMDOG_ASSERT(address_.asV6.sin6_family == AF_INET);

    return AddressFamily::InterNetworkV4;
}

int EndPoint::getPort() const noexcept
{
    if (getFamily() == AddressFamily::InterNetworkV4) {
        return ntohs(address_.asV4.sin_port);
    }
    POMDOG_ASSERT(getFamily() == AddressFamily::InterNetworkV6);
    return ntohs(address_.asV6.sin6_port);
}

std::string EndPoint::toString() const
{
    if (getFamily() == AddressFamily::InterNetworkV6) {
        std::array<char, INET6_ADDRSTRLEN> hostBuf;
        ::inet_ntop(AF_INET6, &address_.asV6.sin6_addr, hostBuf.data(), static_cast<socklen_t>(hostBuf.size()));

        // NOTE: IPv6 (e.g. "[host]:port")
        std::stringstream ss;
        ss << "[" << hostBuf.data() << "]:" << ntohs(address_.asV6.sin6_port);
        return ss.str();
    }

    POMDOG_ASSERT(getFamily() == AddressFamily::InterNetworkV4);
    std::array<char, INET_ADDRSTRLEN> hostBuf;
    ::inet_ntop(AF_INET, &address_.asV4.sin_addr, hostBuf.data(), static_cast<socklen_t>(hostBuf.size()));

    // NOTE: IPv4 (e.g. "host:port")
    std::stringstream ss;
    ss << hostBuf.data() << ":" << ntohs(address_.asV4.sin_port);
    return ss.str();
}

EndPointAddressView EndPoint::getAddressView() const noexcept
{
    if (getFamily() == AddressFamily::InterNetworkV4) {
        EndPointAddressView view;
        view.data = reinterpret_cast<const ::sockaddr*>(&address_.asV4);
        view.size = sizeof(address_.asV4);
        return view;
    }
    POMDOG_ASSERT(getFamily() == AddressFamily::InterNetworkV6);
    EndPointAddressView view;
    view.data = reinterpret_cast<const ::sockaddr*>(&address_.asV6);
    view.size = sizeof(address_.asV6);
    return view;
}

EndPoint EndPoint::createFromAddressStorage(const ::sockaddr_storage& storage)
{
    EndPoint endPoint;
    std::memset(&endPoint.address_, 0, sizeof(endPoint.address_));

    POMDOG_ASSERT(storage.ss_family == AF_INET || storage.ss_family == AF_INET6);
    if (storage.ss_family == AF_INET) {
        endPoint.address_.asV4 = *reinterpret_cast<const ::sockaddr_in*>(&storage);
    }
    else if (storage.ss_family == AF_INET6) {
        endPoint.address_.asV6 = *reinterpret_cast<const ::sockaddr_in6*>(&storage);
    }
    return endPoint;
}

} // namespace pomdog::detail
