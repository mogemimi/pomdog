//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_HASHINGHELPER_HPP
#define POMDOG_HASHINGHELPER_HPP

#include <Pomdog/Config/FundamentalTypes.hpp>
#include "CRC32.hpp"
#include <string>

namespace Pomdog {

struct HashingHelper
{
	static std::uint32_t Hash(std::string const& data)
	{
		return Hashing::CRC32::BlockChecksum(
			reinterpret_cast<std::uint8_t const*>(data.c_str()), data.size());
	}

	static std::uint32_t Hash(void const* data, std::size_t size)
	{
		return Hashing::CRC32::BlockChecksum(
			reinterpret_cast<std::uint8_t const*>(data), size);
	}
};

}// namespace Pomdog

#endif // !defined(POMDOG_HASHINGHELPER_HPP)
