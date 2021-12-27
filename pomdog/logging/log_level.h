// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

enum class LogLevel : std::uint8_t {
    Critical = 0,
    Warning = 1,
    Info = 2,
    Verbose = 3,
    Internal = 4,
};

} // namespace pomdog
