// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Utility/Errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

[[nodiscard]] std::unique_ptr<Error> PrepareNetworkService();

[[nodiscard]] std::unique_ptr<Error> ShutdownNetworkService();

} // namespace Pomdog::Detail::Win32
