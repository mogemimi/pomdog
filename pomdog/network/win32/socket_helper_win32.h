// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/network/socket_protocol.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <WinSock2.h>
#include <expected>
#include <memory>
#include <optional>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::network::detail {

[[nodiscard]] std::expected<::SOCKET, std::unique_ptr<Error>>
connectSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::expected<::SOCKET, std::unique_ptr<Error>>
bindSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace pomdog::network::detail
