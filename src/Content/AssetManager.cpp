// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetManager.hpp"
#include <utility>

namespace Pomdog {

AssetManager::AssetManager(Detail::AssetLoaderContext && loaderContextIn)
    : loaderContext(std::move(loaderContextIn))
{
}

void AssetManager::Unload()
{
    assets.Unload();
}

std::string AssetManager::GetRootDirectory() const
{
    return loaderContext.RootDirectory;
}

Detail::BinaryFileStream AssetManager::OpenStream(const std::string& assetName) const
{
    return loaderContext.OpenStream(assetName);
}

} // namespace Pomdog
