// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Content/detail/AssetLoaders/AudioClipLoader.hpp"
#include "../Utility/MSWaveAudioLoader.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Content/Utility/PathHelper.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {
namespace Detail {
//-----------------------------------------------------------------------
std::shared_ptr<AudioClip> AssetLoader<AudioClip>::operator()(
    AssetLoaderContext const& loaderContext, std::string const& assetName)
{
    std::shared_ptr<AudioClip> audioClip = Detail::MSWaveAudioLoader::Load(
        PathHelper::Join(loaderContext.RootDirectory, assetName));

    return std::move(audioClip);
}
//-----------------------------------------------------------------------
} // namespace Detail
} // namespace Pomdog
