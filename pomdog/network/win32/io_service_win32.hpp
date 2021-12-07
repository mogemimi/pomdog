// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Win32 {

[[nodiscard]] std::unique_ptr<Error> PrepareNetworkService();

[[nodiscard]] std::unique_ptr<Error> ShutdownNetworkService();

} // namespace Pomdog::Detail::Win32
