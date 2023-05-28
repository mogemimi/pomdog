// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/content/asset_manager.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/filesystem/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

AssetManager::AssetManager(
    const std::string& contentDirectoryIn,
    const std::shared_ptr<AudioEngine>& audioEngineIn,
    const std::shared_ptr<gpu::GraphicsDevice>& graphicsDeviceIn)
    : contentDirectory(contentDirectoryIn)
    , audioEngine(audioEngineIn)
    , graphicsDevice(graphicsDeviceIn)
{
}

void AssetManager::Unload()
{
    assets.clear();
}

std::string AssetManager::GetContentDirectory() const noexcept
{
    return contentDirectory;
}

void AssetManager::SetContentDirectory(const std::string& dir) noexcept
{
    contentDirectory = dir;
}

std::shared_ptr<AudioEngine> AssetManager::getAudioEngine() const noexcept
{
    return audioEngine.lock();
}

std::shared_ptr<gpu::GraphicsDevice> AssetManager::getGraphicsDevice() const noexcept
{
    return graphicsDevice.lock();
}

std::string AssetManager::GetAssetPath(const std::string& assetName) const noexcept
{
    if (PathHelper::IsAbsolute(assetName)) {
        return assetName;
    }

    return PathHelper::Join(contentDirectory, assetName);
}

} // namespace pomdog
