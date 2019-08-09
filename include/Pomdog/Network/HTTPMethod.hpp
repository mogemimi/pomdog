// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class HTTPMethod : std::int8_t {
    Get,
    Head,
    Post,
    Put,
    Delete,
    Connect,
    Trace,
};

} // namespace Pomdog
