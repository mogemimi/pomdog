// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "../Utility/PathHelper.hpp"
#include <utility>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
std::ifstream AssetLoaderContext::OpenStream(std::string const& assetName) const
{
	auto path = PathHelper::Join(RootDirectory, assetName);
	std::ifstream stream(path, std::ios::in | std::ios::binary);
	return std::move(stream);
}
//-----------------------------------------------------------------------
}// namespace Detail
}// namespace Pomdog
