// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_LOGLEVEL_5E5C0F27_HPP
#define POMDOG_LOGLEVEL_5E5C0F27_HPP

#include <cstdint>

namespace Pomdog {

enum class LogLevel: std::uint8_t {
    Critical = 0,

    Warning = 1,

    Info = 2,

    Verbose = 3,

    Internal = 4,
};

}// namespace Pomdog

#endif // POMDOG_LOGLEVEL_5E5C0F27_HPP
