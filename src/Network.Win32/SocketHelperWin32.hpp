// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Network/SocketProtocol.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <WinSock2.h>
#include <memory>
#include <optional>
#include <string>
#include <tuple>

namespace Pomdog::Detail {

[[nodiscard]] std::tuple<::SOCKET, std::unique_ptr<Error>>
ConnectSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol, const Duration& timeout);

[[nodiscard]] std::tuple<::SOCKET, std::unique_ptr<Error>>
BindSocketWin32(const std::string& host, const std::string& port, SocketProtocol protocol);

} // namespace Pomdog::Detail
