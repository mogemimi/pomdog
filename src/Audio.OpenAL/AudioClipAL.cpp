// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "AudioClipAL.hpp"
#include "ErrorCheckerAL.hpp"
#include "../Audio/AudioHelper.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <tuple>
#include <utility>

namespace Pomdog::Detail::OpenAL {
namespace {

[[nodiscard]] std::tuple<ALenum, std::unique_ptr<Error>>
ToFormat(AudioChannels channel, int bitPerSample) noexcept
{
    switch (channel) {
    case AudioChannels::Mono:
        if (bitPerSample == 8) {
            return std::make_tuple(AL_FORMAT_MONO8, nullptr);
        }
        if (bitPerSample == 16) {
            return std::make_tuple(AL_FORMAT_MONO16, nullptr);
        }
        break;
    case AudioChannels::Stereo:
        if (bitPerSample == 8) {
            return std::make_tuple(AL_FORMAT_STEREO8, nullptr);
        }
        if (bitPerSample == 16) {
            return std::make_tuple(AL_FORMAT_STEREO16, nullptr);
        }
        break;
    }
    return std::make_tuple(AL_FORMAT_STEREO8, Errors::New("Unsupported audio format"));
}

} // namespace

AudioClipAL::AudioClipAL() noexcept = default;

AudioClipAL::~AudioClipAL() noexcept
{
    if (buffer) {
        alDeleteBuffers(1, &(*buffer));
        POMDOG_CHECK_ERROR_OPENAL("alDeleteBuffers");
    }
}

std::unique_ptr<Error>
AudioClipAL::Initialize(
    const void* data,
    std::size_t sizeInBytesIn,
    int sampleRateIn,
    int bitsPerSampleIn,
    AudioChannels channelsIn) noexcept
{
    this->sizeInBytes = sizeInBytesIn;
    this->sampleRate = sampleRateIn;
    this->bitsPerSample = bitsPerSampleIn;
    this->channels = channelsIn;

    POMDOG_ASSERT(bitsPerSample == 8 || bitsPerSample == 16);

    buffer = [] {
        ALuint nativeBuffer;
        alGenBuffers(1, &nativeBuffer);
        return std::move(nativeBuffer);
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alGenBuffers() failed.");
    }

    POMDOG_ASSERT(buffer != std::nullopt);
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto [format, formatErr] = ToFormat(channels, bitsPerSample);
    if (formatErr != nullptr) {
        return Errors::Wrap(std::move(formatErr), "ToFormat() failed.");
    }

    alBufferData(*buffer, format, data, static_cast<ALsizei>(sizeInBytes), sampleRate);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alBufferData() failed.");
    }

    return nullptr;
}

Duration AudioClipAL::GetLength() const noexcept
{
    auto samples = Detail::AudioHelper::GetSamples(sizeInBytes, bitsPerSample, channels);
    auto sampleDuration = Detail::AudioHelper::GetSampleDuration(samples, sampleRate);
    return sampleDuration;
}

int AudioClipAL::GetSampleRate() const noexcept
{
    return sampleRate;
}

int AudioClipAL::GetBitsPerSample() const noexcept
{
    return bitsPerSample;
}

AudioChannels AudioClipAL::GetChannels() const noexcept
{
    return channels;
}

std::size_t AudioClipAL::GetSizeInBytes() const noexcept
{
    return sizeInBytes;
}

ALuint AudioClipAL::GetNativeBuffer() const noexcept
{
    POMDOG_ASSERT(buffer != std::nullopt);
    return *buffer;
}

} // namespace Pomdog::Detail::OpenAL
