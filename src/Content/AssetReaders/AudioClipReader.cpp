//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AudioClipReader.hpp"
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
std::shared_ptr<AudioClip> AudioClipReader::Read(
	AssetLoaderContext const& loaderContext, std::string const& assetName)
{
	std::shared_ptr<AudioClip> audioClip = Details::MSWaveAudioLoader::Load(
		PathHelper::Join(loaderContext.RootDirectory, assetName));

	return std::move(audioClip);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
