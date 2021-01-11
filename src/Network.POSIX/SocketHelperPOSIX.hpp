// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Network/SocketProtocol.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <string>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail {

[[nodiscard]] std::tuple<int, std::unique_ptr<Error>>
ConnectSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::tuple<int, std::unique_ptr<Error>>
BindSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace Pomdog::Detail
