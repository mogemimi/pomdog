// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/network/socket_protocol.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

[[nodiscard]] std::tuple<::SOCKET, std::unique_ptr<Error>>
ConnectSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::tuple<::SOCKET, std::unique_ptr<Error>>
BindSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace pomdog::detail
