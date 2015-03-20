//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Audio/AudioClip.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace {

static std::size_t GetSamples(std::size_t sizeInBytes,
	std::uint16_t bitsPerSample, AudioChannels channels)
{
	POMDOG_ASSERT(bitsPerSample >= 8);
	POMDOG_ASSERT(bitsPerSample == 8
		|| bitsPerSample == 16
		|| bitsPerSample == 24
		|| bitsPerSample == 32);

	static_assert(static_cast<int>(AudioChannels::Mono) == 1, "");
	static_assert(static_cast<int>(AudioChannels::Stereo) == 2, "");

	auto channelCount = static_cast<int>(channels);
	POMDOG_ASSERT(channelCount > 0);
	POMDOG_ASSERT(channelCount <= 2);

	auto divisior = (bitsPerSample / 8) * channelCount;
	POMDOG_ASSERT(divisior > 0);

	return sizeInBytes / divisior;
}
//-----------------------------------------------------------------------
static Duration GetSampleDuration(std::size_t samples, std::uint32_t sampleRate)
{
	POMDOG_ASSERT(sampleRate > 0);
	POMDOG_ASSERT(sampleRate >= 8000);
	POMDOG_ASSERT(sampleRate <= 48000);
	return std::chrono::seconds(samples / sampleRate);
}

}// unnamed namespace
//-----------------------------------------------------------------------
AudioClip::AudioClip(std::unique_ptr<Detail::SoundSystem::NativeAudioClip> && nativeAudioClipIn,
	std::uint32_t sampleRateIn, std::uint16_t bitsPerSampleIn, AudioChannels channelsIn)
	: nativeAudioClip(std::move(nativeAudioClipIn))
	, sampleRate(sampleRateIn)
	, bitsPerSample(bitsPerSampleIn)
	, channels(channelsIn)
{
}
//-----------------------------------------------------------------------
AudioClip::~AudioClip() = default;
//-----------------------------------------------------------------------
Duration AudioClip::Length() const
{
	POMDOG_ASSERT(nativeAudioClip);
	auto samples = GetSamples(nativeAudioClip->SizeInBytes(), bitsPerSample, channels);
	auto sampleDuration = GetSampleDuration(samples, sampleRate);
	return std::move(sampleDuration);
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
Detail::SoundSystem::NativeAudioClip* AudioClip::NativeAudioClip()
{
	POMDOG_ASSERT(nativeAudioClip);
	return nativeAudioClip.get();
}
//-----------------------------------------------------------------------
}// namespace Pomdog
