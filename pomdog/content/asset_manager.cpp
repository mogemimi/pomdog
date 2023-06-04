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
    : contentDirectory_(contentDirectoryIn)
    , audioEngine_(audioEngineIn)
    , graphicsDevice_(graphicsDeviceIn)
{
}

void AssetManager::unload()
{
    assets_.clear();
}

std::string AssetManager::getContentDirectory() const noexcept
{
    return contentDirectory_;
}

void AssetManager::setContentDirectory(const std::string& dir) noexcept
{
    contentDirectory_ = dir;
}

std::shared_ptr<AudioEngine> AssetManager::getAudioEngine() const noexcept
{
    return audioEngine_.lock();
}

std::shared_ptr<gpu::GraphicsDevice> AssetManager::getGraphicsDevice() const noexcept
{
    return graphicsDevice_.lock();
}

std::string AssetManager::getAssetPath(const std::string& assetName) const noexcept
{
    if (filepaths::isAbsolute(assetName)) {
        return assetName;
    }

    return filepaths::join(contentDirectory_, assetName);
}

} // namespace pomdog
