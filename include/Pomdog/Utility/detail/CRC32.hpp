//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_UTILITY_CRC32_7EBEE5EE_0A7A_4875_BBF4_48CAF21443FF_HPP
#define POMDOG_UTILITY_CRC32_7EBEE5EE_0A7A_4875_BBF4_48CAF21443FF_HPP

#include "Pomdog/Config/Export.hpp"
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

#endif // !defined(POMDOG_UTILITY_CRC32_7EBEE5EE_0A7A_4875_BBF4_48CAF21443FF_HPP)
