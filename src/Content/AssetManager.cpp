// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/AssetManager.hpp"
#include <utility>

namespace Pomdog {
//-----------------------------------------------------------------------
AssetManager::AssetManager(Detail::AssetLoaderContext && loaderContextIn)
    : loaderContext(std::move(loaderContextIn))
{}
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
Detail::BinaryFileStream AssetManager::OpenStream(std::string const& assetName) const
{
    return loaderContext.OpenStream(assetName);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
