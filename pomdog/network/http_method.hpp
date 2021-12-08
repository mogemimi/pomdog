// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// HTTPMethod represents comman HTTP methods.
enum class HTTPMethod : std::int8_t {
    /// Represents a GET protocol method.
    Get,

    /// Represents a HEAD protocol method.
    Head,

    /// Represents a POST protocol method.
    Post,

    /// Represents a PUT protocol method.
    Put,

    /// Represents a DELETE protocol method.
    Delete,

    /// Represents a CONNECT protocol method.
    Connect,

    /// Represents a TRACE protocol method.
    Trace,
};

} // namespace pomdog
