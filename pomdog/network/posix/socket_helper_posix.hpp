// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/network/socket_protocol.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail {

[[nodiscard]] std::tuple<int, std::unique_ptr<Error>>
ConnectSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::tuple<int, std::unique_ptr<Error>>
BindSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace pomdog::detail
