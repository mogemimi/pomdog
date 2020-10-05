// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Audio/AudioClip.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_LINUX)
#include "../SoundSystem.OpenAL/AudioClipAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/AudioClipXAudio2.hpp"
#else
#error "Platform undefined or not supported."
#endif

#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace {

std::size_t GetSamples(
    std::size_t sizeInBytes, int bitsPerSample, AudioChannels channels)
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

Duration GetSampleDuration(std::size_t samples, int sampleRate)
{
    POMDOG_ASSERT(sampleRate > 0);
    POMDOG_ASSERT(sampleRate >= 8000);
    POMDOG_ASSERT(sampleRate <= 48000);
    return std::chrono::seconds(samples / sampleRate);
}

} // unnamed namespace

AudioClip::AudioClip(std::unique_ptr<Detail::SoundSystem::NativeAudioClip>&& nativeAudioClipIn,
    int sampleRateIn, int bitsPerSampleIn, AudioChannels channelsIn)
    : nativeAudioClip(std::move(nativeAudioClipIn))
    , sampleRate(sampleRateIn)
    , bitsPerSample(bitsPerSampleIn)
    , channels(channelsIn)
{
}

AudioClip::~AudioClip() = default;

Duration AudioClip::GetLength() const
{
    POMDOG_ASSERT(nativeAudioClip);
    auto samples = GetSamples(nativeAudioClip->SizeInBytes(), bitsPerSample, channels);
    auto sampleDuration = GetSampleDuration(samples, sampleRate);
    return sampleDuration;
}

int AudioClip::GetSampleRate() const
{
    return sampleRate;
}

int AudioClip::GetBitsPerSample() const
{
    return bitsPerSample;
}

AudioChannels AudioClip::GetChannels() const
{
    return channels;
}

Detail::SoundSystem::NativeAudioClip* AudioClip::GetNativeAudioClip()
{
    POMDOG_ASSERT(nativeAudioClip);
    return nativeAudioClip.get();
}

} // namespace Pomdog
