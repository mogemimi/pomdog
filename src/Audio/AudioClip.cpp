//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "Pomdog/Audio/AudioClip.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#endif

#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
AudioClip::AudioClip(void const* data, std::size_t size,
	DurationSeconds durationIn, std::uint32_t sampleRateIn, std::uint16_t bitsPerSampleIn, AudioChannels channelsIn)
	: nativeAudioClip(std::make_unique<Details::SoundSystem::NativeAudioClip>(data, size, channelsIn, bitsPerSampleIn, sampleRateIn))
	, duration(durationIn)
	, sampleRate(sampleRateIn)
	, bitsPerSample(bitsPerSampleIn)
	, channels(channelsIn)
{
}
//-----------------------------------------------------------------------
AudioClip::~AudioClip() = default;
//-----------------------------------------------------------------------
DurationSeconds AudioClip::Duration() const
{
	return duration;
}
//-----------------------------------------------------------------------
std::uint32_t AudioClip::SampleRate() const
{
	return sampleRate;
}
//-----------------------------------------------------------------------
std::uint16_t AudioClip::BitsPerSample() const
{
	return bitsPerSample;
}
//-----------------------------------------------------------------------
AudioChannels AudioClip::Channels() const
{
	return channels;
}
//-----------------------------------------------------------------------
Details::SoundSystem::NativeAudioClip* AudioClip::NativeAudioClip()
{
	POMDOG_ASSERT(nativeAudioClip);
	return nativeAudioClip.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
