//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_AUDIOBUFFER_C1887233_F337_44F3_8AAC_784260497BCE_HPP
#define POMDOG_AUDIOBUFFER_C1887233_F337_44F3_8AAC_784260497BCE_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "detail/ForwardDeclarations.hpp"
#include "AudioChannels.hpp"
#include "Pomdog/Application/DurationSeconds.hpp"
#include "Pomdog/Config/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

///@~Japanese
/// @brief オーディオバッファです。
class POMDOG_EXPORT AudioBuffer {
public:
	AudioBuffer(void const* data, std::size_t size, DurationSeconds duration,
		std::uint32_t sampleRate, std::uint16_t bitsPerSample, AudioChannels channels);
	
	AudioBuffer(AudioBuffer const&) = delete;
	AudioBuffer & operator=(AudioBuffer const&) = delete;

	~AudioBuffer();

	///@~Japanese
	/// @brief ソースバッファの全長（秒単位）。
	DurationSeconds Duration() const;
	
	///@~Japanese
	/// @brief サンプリングレート（1秒あたりのサンプル数）。
	std::uint32_t SampleRate() const;
	
	///@~Japanese
	/// @brief サンプルあたりのビット数。
	std::uint16_t BitsPerSample() const;

	AudioChannels Channels() const;
	
	Details::SoundSystem::NativeAudioBuffer* NativeAudioBuffer();
	
private:
	std::unique_ptr<Details::SoundSystem::NativeAudioBuffer> nativeAudioBuffer;
	DurationSeconds duration;
	std::uint32_t sampleRate;
	std::uint16_t bitsPerSample;
	AudioChannels channels;
};

}// namespace Pomdog

#endif // !defined(POMDOG_AUDIOBUFFER_C1887233_F337_44F3_8AAC_784260497BCE_HPP)
