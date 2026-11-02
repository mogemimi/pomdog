// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/network/win32/winsock_win32.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

WinsockLibrary::WinsockLibrary() noexcept = default;

WinsockLibrary::~WinsockLibrary()
{
    if (started_) {
        ::WSACleanup();
        started_ = false;
    }
}

std::expected<void, std::unique_ptr<Error>>
WinsockLibrary::startup()
{
    POMDOG_ASSERT(!started_);

    WSADATA wsaData;
    if (const auto result = ::WSAStartup(MAKEWORD(2, 2), &wsaData); result != 0) {
        return std::unexpected(errors::make("WSAStartup failed with error " + std::to_string(result)));
    }
    started_ = true;
    return {};
}

} // namespace pomdog::network::detail
