// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_clip_xaudio2.hpp"
#include "pomdog/audio/audio_helper.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::XAudio2 {
namespace {

[[nodiscard]] int GetChannelCount(AudioChannels channels) noexcept
{
    static_assert(static_cast<int>(AudioChannels::Mono) == 1);
    static_assert(static_cast<int>(AudioChannels::Stereo) == 2);
    return static_cast<int>(channels);
}

} // namespace

AudioClipXAudio2::AudioClipXAudio2() noexcept = default;

std::unique_ptr<Error>
AudioClipXAudio2::Initialize(
    const void* audioDataIn,
    std::size_t sizeInBytes,
    int sampleRate,
    int bitsPerSample,
    AudioChannels channelsIn) noexcept
{
    POMDOG_ASSERT(audioDataIn != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    this->channels = channelsIn;

    this->audioData.resize(sizeInBytes);
    std::memcpy(audioData.data(), audioDataIn, sizeInBytes);

    const auto channelCount = GetChannelCount(this->channels);
    const auto blockAlign = channelCount * (bitsPerSample / 8);

    this->waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    this->waveFormat.nChannels = static_cast<WORD>(channelCount);
    this->waveFormat.nSamplesPerSec = sampleRate;
    this->waveFormat.nAvgBytesPerSec = sampleRate * blockAlign;
    this->waveFormat.nBlockAlign = static_cast<WORD>(blockAlign);
    this->waveFormat.wBitsPerSample = static_cast<WORD>(bitsPerSample);
    this->waveFormat.cbSize = 0;

    const auto samples = Detail::AudioHelper::GetSamples(sizeInBytes, bitsPerSample, channels);
    this->sampleDuration = Detail::AudioHelper::GetSampleDuration(samples, sampleRate);

    return nullptr;
}

Duration
AudioClipXAudio2::GetLength() const noexcept
{
    return sampleDuration;
}

int AudioClipXAudio2::GetSampleRate() const noexcept
{
    return waveFormat.nSamplesPerSec;
}

int AudioClipXAudio2::GetBitsPerSample() const noexcept
{
    return waveFormat.wBitsPerSample;
}

AudioChannels
AudioClipXAudio2::GetChannels() const noexcept
{
    return channels;
}

const WAVEFORMATEX* AudioClipXAudio2::GetWaveFormat() const noexcept
{
    return &waveFormat;
}

const std::uint8_t* AudioClipXAudio2::GetData() const noexcept
{
    POMDOG_ASSERT(!audioData.empty());
    return audioData.data();
}

std::size_t AudioClipXAudio2::GetSizeInBytes() const noexcept
{
    POMDOG_ASSERT(!audioData.empty());
    return sizeof(std::uint8_t) * audioData.size();
}

} // namespace Pomdog::Detail::XAudio2
