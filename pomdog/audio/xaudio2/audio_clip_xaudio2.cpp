// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/audio/audio_helper.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {
namespace {

[[nodiscard]] int
getChannelCount(AudioChannels channels) noexcept
{
    static_assert(static_cast<int>(AudioChannels::Mono) == 1);
    static_assert(static_cast<int>(AudioChannels::Stereo) == 2);
    return static_cast<int>(channels);
}

} // namespace

AudioClipXAudio2::AudioClipXAudio2() noexcept = default;

std::unique_ptr<Error>
AudioClipXAudio2::initialize(
    const void* audioDataIn,
    std::size_t sizeInBytes,
    int sampleRate,
    int bitsPerSample,
    AudioChannels channelsIn) noexcept
{
    POMDOG_ASSERT(audioDataIn != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    channels_ = channelsIn;

    audioData_.resize(sizeInBytes);
    std::memcpy(audioData_.data(), audioDataIn, sizeInBytes);

    const auto channelCount = getChannelCount(channels_);
    const auto blockAlign = channelCount * (bitsPerSample / 8);

    waveFormat_.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat_.nChannels = static_cast<WORD>(channelCount);
    waveFormat_.nSamplesPerSec = sampleRate;
    waveFormat_.nAvgBytesPerSec = sampleRate * blockAlign;
    waveFormat_.nBlockAlign = static_cast<WORD>(blockAlign);
    waveFormat_.wBitsPerSample = static_cast<WORD>(bitsPerSample);
    waveFormat_.cbSize = 0;

    const auto samples = detail::AudioHelper::getSamples(sizeInBytes, bitsPerSample, channels_);
    sampleDuration_ = detail::AudioHelper::getSampleDuration(samples, sampleRate);

    return nullptr;
}

Duration
AudioClipXAudio2::getLength() const noexcept
{
    return sampleDuration_;
}

int AudioClipXAudio2::getSampleRate() const noexcept
{
    return waveFormat_.nSamplesPerSec;
}

int AudioClipXAudio2::getBitsPerSample() const noexcept
{
    return waveFormat_.wBitsPerSample;
}

AudioChannels
AudioClipXAudio2::getChannels() const noexcept
{
    return channels_;
}

const WAVEFORMATEX*
AudioClipXAudio2::getWaveFormat() const noexcept
{
    return &waveFormat_;
}

const std::uint8_t*
AudioClipXAudio2::getData() const noexcept
{
    POMDOG_ASSERT(!audioData_.empty());
    return audioData_.data();
}

std::size_t
AudioClipXAudio2::getSizeInBytes() const noexcept
{
    POMDOG_ASSERT(!audioData_.empty());
    return sizeof(std::uint8_t) * audioData_.size();
}

} // namespace pomdog::detail::xaudio2
