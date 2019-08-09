// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Errors.hpp"
#include <functional>
#include <memory>

namespace Pomdog::Detail::Win32 {

[[nodiscard]] std::shared_ptr<Error> PrepareNetworkService();

[[nodiscard]] std::shared_ptr<Error> ShutdownNetworkService();

} // namespace Pomdog::Detail::Win32
