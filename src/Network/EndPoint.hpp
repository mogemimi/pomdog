// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "AddressFamily.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Basic/Platform.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <string>

#if defined(POMDOG_PLATFORM_MACOSX) || \
    defined(POMDOG_PLATFORM_APPLE_IOS) || \
    defined(POMDOG_PLATFORM_LINUX)
#include <netinet/in.h>
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include <WS2tcpip.h>
#include <WinSock2.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

struct EndPointAddressView final {
    const ::sockaddr* data = nullptr;
    ::socklen_t size = 0;
};

/// EndPoint represents the address of a network end point.
class EndPoint final {
public:
    EndPoint();

    /// Gets the address family.
    [[nodiscard]] AddressFamily GetFamily() const noexcept;

    /// Gets the port number of the endpoint.
    [[nodiscard]] int GetPort() const noexcept;

    /// Returns a string representation of the endpoint address.
    [[nodiscard]] std::string ToString() const;

    /// Gets a view of the endpoint address.
    [[nodiscard]] EndPointAddressView GetAddressView() const noexcept;

    /// Creates an endpoint from `sockaddr_storage` directly.
    [[nodiscard]] static EndPoint
    CreateFromAddressStorage(const ::sockaddr_storage& storage);

private:
    union {
        ::sockaddr_in asV4;
        ::sockaddr_in6 asV6;
    } address;
};

} // namespace Pomdog::Detail
