// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "IOServiceWin32.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

std::unique_ptr<Error> PrepareNetworkService()
{
    WSADATA wsaData;

    const auto result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        auto err = Errors::New("WSAStartup failed with error " + std::to_string(result));
        return err;
    }
    return nullptr;
}

std::unique_ptr<Error> ShutdownNetworkService()
{
    const auto result = ::WSACleanup();
    if (result != 0) {
        auto err = Errors::New("WSACleanup failed with error " + std::to_string(result));
        return err;
    }
    return nullptr;
}

} // namespace Pomdog::Detail::Win32
