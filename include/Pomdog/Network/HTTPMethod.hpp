// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

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

} // namespace Pomdog
