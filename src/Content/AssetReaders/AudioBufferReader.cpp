//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AudioBufferReader.hpp"
#include "Pomdog/Content/detail/AssetLoaderContext.hpp"
#include "Pomdog/Audio/AudioBuffer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

#include "Pomdog/Config/Platform.hpp"
#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../../Utility/PathHelper.hpp"
#include "../Utility/MSWaveAudioLoader.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#error "TODO: Not implemented"
#endif

#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace Pomdog {
namespace Details {
//-----------------------------------------------------------------------
std::shared_ptr<AudioBuffer> AudioBufferReader::Read(
	AssetLoaderContext const& loaderContext, std::string const& assetName)
{
	auto source = Details::MSWaveAudioLoader::Load(
		PathHelper::Join(loaderContext.RootDirectory, assetName));

	auto audioBuffer = std::make_shared<AudioBuffer>(source.Data.data(), source.Data.size(),
		source.Duration, source.SampleRate, source.BitsPerSample, source.Channels);

	return std::move(audioBuffer);
}
//-----------------------------------------------------------------------
}// namespace Details
}// namespace Pomdog
