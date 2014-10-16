//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "PathHelper.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
std::string PathHelper::Join(std::string const& path1, std::string const& path2)
{
	std::string result = path1;

	if (!path1.empty() && '/' != path1.back()) {
		if (!path2.empty() && '/' != path2.front()) {
			result += '/';
		}
	}

	result += path2;
	return std::move(result);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
