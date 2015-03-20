//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Content/AssetLoaders/EffectPassLoader.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
AssetManager::AssetManager(Detail::AssetLoaderContext && loaderContextIn)
	: loaderContext(std::move(loaderContextIn))
{}
//-----------------------------------------------------------------------
AssetLoaders::EffectPassLoader AssetManager::LoadEffectPass()
{
	AssetLoaders::EffectPassLoader loader(loaderContext);
	return std::move(loader);
}
//-----------------------------------------------------------------------
void AssetManager::Unload()
{
	assets.Unload();
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
