// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::network {

/// HTTPMethod represents comman HTTP methods.
enum class HTTPMethod : u8 {
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

    /// Represents a CONNECT protocol method. A request with it fails on every
    /// platform, because browsers do not let fetch() send it.
    Connect,

    /// Represents a TRACE protocol method. A request with it fails on every
    /// platform, because browsers do not let fetch() send it.
    Trace,
};

} // namespace pomdog::network
