//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Content/AssetManager.hpp>

namespace Pomdog {
//-----------------------------------------------------------------------
AssetManager::AssetManager(Details::AssetLoaderContext && loaderContextIn)
	: loaderContext(std::move(loaderContextIn))
{}
//-----------------------------------------------------------------------
void AssetManager::Unload()
{
	effectPassAssets.Unload();
	texture2dAssets.Unload();
}
//-----------------------------------------------------------------------
std::string AssetManager::RootDirectory() const
{
	return loaderContext.RootDirectory;
}
//-----------------------------------------------------------------------
std::ifstream AssetManager::OpenStream(std::string const& assetName) const
{
	return loaderContext.OpenStream(assetName);
}
//-----------------------------------------------------------------------
}// namespace Pomdog
