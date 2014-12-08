//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP
#define POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "AudioChannels.hpp"
#include "DurationSeconds.hpp"
#include <cstdint>
#include <vector>
#include <string>

namespace Pomdog {

class AudioClip;

namespace Details {

class AudioClipSource {
public:
	std::vector<std::uint8_t> Data;
	///@~Japanese
	/// @brief ソースバッファの全長（秒単位）。
	DurationSeconds Duration;
	
	///@~Japanese
	/// @brief サンプリングレート（1秒あたりのサンプル数）。
	std::uint32_t SampleRate;
	
	///@~Japanese
	/// @brief サンプルあたりのビット数。
	std::uint16_t BitsPerSample;

	AudioChannels Channels;
};

struct MSWaveAudioLoader {
	static AudioClipSource Load(std::string const& filePath);
};

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_MSWAVEAUDIOLOADER_8808FBBE_6F7C_4FB1_84C9_CB1833D51283_HPP)
