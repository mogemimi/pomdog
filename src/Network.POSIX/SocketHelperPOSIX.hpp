// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Network/SocketProtocol.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <memory>
#include <optional>
#include <string>
#include <tuple>

namespace Pomdog::Detail {

[[nodiscard]] std::tuple<int, std::shared_ptr<Error>>
ConnectSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::tuple<int, std::shared_ptr<Error>>
BindSocketPOSIX(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace Pomdog::Detail
