//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "AudioBuffer.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioBufferAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioBufferXAudio2.hpp"
#endif

#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
AudioBuffer::AudioBuffer(void const* data, std::size_t size,
	DurationSeconds durationIn, std::uint32_t sampleRateIn, std::uint16_t bitsPerSampleIn, AudioChannels channelsIn)
	: nativeAudioBuffer(std::make_unique<Details::SoundSystem::NativeAudioBuffer>(data, size, channelsIn, bitsPerSampleIn, sampleRateIn))
	, duration(durationIn)
	, sampleRate(sampleRateIn)
	, bitsPerSample(bitsPerSampleIn)
	, channels(channelsIn)
{
}
//-----------------------------------------------------------------------
AudioBuffer::~AudioBuffer() = default;
//-----------------------------------------------------------------------
DurationSeconds AudioBuffer::Duration() const
{
	return duration;
}
//-----------------------------------------------------------------------
std::uint32_t AudioBuffer::SampleRate() const
{
	return sampleRate;
}
//-----------------------------------------------------------------------
std::uint16_t AudioBuffer::BitsPerSample() const
{
	return bitsPerSample;
}
//-----------------------------------------------------------------------
AudioChannels AudioBuffer::Channels() const
{
	return channels;
}
//-----------------------------------------------------------------------
Details::SoundSystem::NativeAudioBuffer* AudioBuffer::NativeAudioBuffer()
{
	POMDOG_ASSERT(nativeAudioBuffer);
	return nativeAudioBuffer.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
