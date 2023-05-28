// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/io_service_win32.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

std::unique_ptr<Error> PrepareNetworkService()
{
    WSADATA wsaData;

    const auto result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        auto err = errors::make("WSAStartup failed with error " + std::to_string(result));
        return err;
    }
    return nullptr;
}

std::unique_ptr<Error> ShutdownNetworkService()
{
    const auto result = ::WSACleanup();
    if (result != 0) {
        auto err = errors::make("WSACleanup failed with error " + std::to_string(result));
        return err;
    }
    return nullptr;
}

} // namespace pomdog::detail::win32
