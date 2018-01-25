// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class LogLevel : std::uint8_t {
    Critical = 0,
    Warning = 1,
    Info = 2,
    Verbose = 3,
    Internal = 4,
};

} // namespace Pomdog
