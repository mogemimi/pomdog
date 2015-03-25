// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BUFFERUSAGE_E289E712_AF1F_4E88_A9E7_2E7BB5F1CB33_HPP
#define POMDOG_BUFFERUSAGE_E289E712_AF1F_4E88_A9E7_2E7BB5F1CB33_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class BufferUsage: std::uint8_t {
    Immutable,
    Dynamic,
};

}// namespace Pomdog

#endif // !defined(POMDOG_BUFFERUSAGE_E289E712_AF1F_4E88_A9E7_2E7BB5F1CB33_HPP)
