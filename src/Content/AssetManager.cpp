// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Content/AssetManager.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include <utility>

namespace Pomdog {

AssetManager::AssetManager(
    const std::string& contentDirectoryIn,
    const std::shared_ptr<GraphicsDevice>& graphicsDeviceIn)
    : contentDirectory(contentDirectoryIn)
    , graphicsDevice(graphicsDeviceIn)
{
}

void AssetManager::Unload()
{
    assets.clear();
}

std::string AssetManager::GetContentDirectory() const
{
    return contentDirectory;
}

void AssetManager::SetContentDirectory(const std::string& dir)
{
    contentDirectory = dir;
}

std::shared_ptr<GraphicsDevice> AssetManager::GetGraphicsDevice() const
{
    return graphicsDevice.lock();
}

std::string AssetManager::GetAssetPath(const std::string& assetName) const
{
    if (PathHelper::IsAbsolute(assetName)) {
        return assetName;
    }

    return PathHelper::Join(contentDirectory, assetName);
}

} // namespace Pomdog
