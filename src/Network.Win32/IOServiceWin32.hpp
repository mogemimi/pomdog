// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Utility/Errors.hpp"
#include <functional>
#include <memory>

namespace Pomdog::Detail::Win32 {

[[nodiscard]] std::unique_ptr<Error> PrepareNetworkService();

[[nodiscard]] std::unique_ptr<Error> ShutdownNetworkService();

} // namespace Pomdog::Detail::Win32
