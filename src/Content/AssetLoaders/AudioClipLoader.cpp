//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Content/detail/AssetLoaders/AudioClipLoader.hpp"
#include "../../Utility/PathHelper.hpp"
#include "../Utility/MSWaveAudioLoader.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
std::shared_ptr<AudioClip> AssetLoader<AudioClip>::operator()(
	AssetLoaderContext const& loaderContext, std::string const& assetName)
{
	std::shared_ptr<AudioClip> audioClip = Details::MSWaveAudioLoader::Load(
		PathHelper::Join(loaderContext.RootDirectory, assetName));

	return std::move(audioClip);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
