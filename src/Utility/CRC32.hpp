//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_CRC32_HPP
#define POMDOG_SRC_CRC32_HPP

#include <Pomdog/Config/FundamentalTypes.hpp>

namespace Pomdog {
namespace Hashing {

// CRC-32 (Cyclic Redundancy Check for 32bit)
namespace CRC32
{
	std::uint32_t BlockChecksum(std::uint8_t const* data, std::size_t length);
}

}// namespace Hashing
}// namespace Pomdof

#endif // !defined(POMDOG_SRC_CRC32_HPP)
