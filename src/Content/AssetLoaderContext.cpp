//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "../Utility/PathHelper.hpp"
#include <utility>

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
std::ifstream AssetLoaderContext::OpenStream(std::string const& assetName) const
{
	auto path = PathHelper::Join(RootDirectory, assetName);
	std::ifstream stream(path, std::ios::in | std::ios::binary);
	return std::move(stream);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
