// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_CRC32_7EBEE5EE_HPP
#define POMDOG_CRC32_7EBEE5EE_HPP

#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <cstddef>

namespace Pomdog {
namespace Hashing {

// CRC-32 (Cyclic Redundancy Check for 32bit)
namespace CRC32 {
    std::uint32_t POMDOG_EXPORT BlockChecksum(void const* data, std::size_t length);
}

}// namespace Hashing
}// namespace Pomdof

#endif // POMDOG_CRC32_7EBEE5EE_HPP
