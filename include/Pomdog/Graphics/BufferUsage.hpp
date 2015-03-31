// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BUFFERUSAGE_E289E712_HPP
#define POMDOG_BUFFERUSAGE_E289E712_HPP

#include <cstdint>

namespace Pomdog {

enum class BufferUsage: std::uint8_t {
    Immutable,
    Dynamic,
};

} // namespace Pomdog

#endif // POMDOG_BUFFERUSAGE_E289E712_HPP
