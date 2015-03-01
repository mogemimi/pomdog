//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "TextureHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>

namespace Pomdog {
namespace Detail {

std::int32_t TextureHelper::ComputeMipmapLevelCount(std::int32_t width, std::int32_t height)
{
	POMDOG_ASSERT(width >= 0);
	POMDOG_ASSERT(height >= 0);

	auto size = std::max(width, height);
	std::int32_t levelCount = 1;

	POMDOG_ASSERT(size >= 0);

	while (size > 1)
	{
		size = size / 2;
		++levelCount;
	}
	return levelCount;
}

}// namespace Detail
}// namespace Pomdog
