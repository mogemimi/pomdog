//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SRC_UTILITY_HASHINGHELPER_E07C1EDA_1E17_4606_B969_9B8679E00806_HPP
#define POMDOG_SRC_UTILITY_HASHINGHELPER_E07C1EDA_1E17_4606_B969_9B8679E00806_HPP

#include <string>
#include <Pomdog/Config/FundamentalTypes.hpp>
#include "CRC32.hpp"

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

#endif // !defined(POMDOG_SRC_UTILITY_HASHINGHELPER_E07C1EDA_1E17_4606_B969_9B8679E00806_HPP)
