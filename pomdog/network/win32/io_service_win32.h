// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::win32 {

[[nodiscard]] std::unique_ptr<Error>
prepareNetworkService();

[[nodiscard]] std::unique_ptr<Error>
shutdownNetworkService();

} // namespace pomdog::detail::win32
