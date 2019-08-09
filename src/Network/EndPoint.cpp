// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#include "EndPoint.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cstring>
#include <sstream>

#if defined(POMDOG_PLATFORM_MACOSX) \
    || defined(POMDOG_PLATFORM_APPLE_IOS) \
    || defined(POMDOG_PLATFORM_LINUX)
#include <arpa/inet.h>
#include <netdb.h>
#endif

namespace Pomdog::Detail {

EndPoint::EndPoint()
{
    std::memset(&address, 0, sizeof(address));
    address.asV6.sin6_family = AF_INET6;
}

AddressFamily EndPoint::GetFamily() const noexcept
{
    if (address.asV6.sin6_family == AF_INET6) {
        POMDOG_ASSERT(address.asV4.sin_family == AF_INET6);
        return AddressFamily::InterNetworkV6;
    }
    POMDOG_ASSERT(address.asV4.sin_family == AF_INET);
    POMDOG_ASSERT(address.asV6.sin6_family == AF_INET);

    return AddressFamily::InterNetworkV4;
}

int EndPoint::GetPort() const noexcept
{
    if (this->GetFamily() == AddressFamily::InterNetworkV4) {
        return ntohs(address.asV4.sin_port);
    }
    POMDOG_ASSERT(this->GetFamily() == AddressFamily::InterNetworkV6);
    return ntohs(address.asV6.sin6_port);
}

std::string EndPoint::ToString() const
{
    if (this->GetFamily() == AddressFamily::InterNetworkV6) {
        std::array<char, INET6_ADDRSTRLEN> hostBuf;
        ::inet_ntop(AF_INET6, &address.asV6.sin6_addr, hostBuf.data(), static_cast<socklen_t>(hostBuf.size()));

        // NOTE: IPv6 (e.g. "[host]:port")
        std::stringstream ss;
        ss << "[" << hostBuf.data() << "]:" << ntohs(address.asV6.sin6_port);
        return ss.str();
    }

    POMDOG_ASSERT(this->GetFamily() == AddressFamily::InterNetworkV4);
    std::array<char, INET_ADDRSTRLEN> hostBuf;
    ::inet_ntop(AF_INET, &address.asV4.sin_addr, hostBuf.data(), static_cast<socklen_t>(hostBuf.size()));

    // NOTE: IPv4 (e.g. "host:port")
    std::stringstream ss;
    ss << hostBuf.data() << ":" << ntohs(address.asV4.sin_port);
    return ss.str();
}

EndPointAddressView EndPoint::GetAddressView() const noexcept
{
    if (this->GetFamily() == AddressFamily::InterNetworkV4) {
        EndPointAddressView view;
        view.data = reinterpret_cast<const ::sockaddr*>(&address.asV4);
        view.size = sizeof(address.asV4);
        return view;
    }
    POMDOG_ASSERT(this->GetFamily() == AddressFamily::InterNetworkV6);
    EndPointAddressView view;
    view.data = reinterpret_cast<const ::sockaddr*>(&address.asV6);
    view.size = sizeof(address.asV6);
    return view;
}

EndPoint EndPoint::CreateFromAddressStorage(const ::sockaddr_storage& storage)
{
    EndPoint endPoint;
    std::memset(&endPoint.address, 0, sizeof(endPoint.address));

    POMDOG_ASSERT(storage.ss_family == AF_INET || storage.ss_family == AF_INET6);
    if (storage.ss_family == AF_INET) {
        endPoint.address.asV4 = *reinterpret_cast<const ::sockaddr_in*>(&storage);
    }
    else if (storage.ss_family == AF_INET6) {
        endPoint.address.asV6 = *reinterpret_cast<const ::sockaddr_in6*>(&storage);
    }
    return endPoint;
}

} // namespace Pomdog::Detail
