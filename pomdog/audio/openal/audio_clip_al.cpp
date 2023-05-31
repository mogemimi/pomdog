// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/audio_helper.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include <tuple>
#include <utility>

namespace pomdog::detail::openal {
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
    return std::make_tuple(AL_FORMAT_STEREO8, errors::make("Unsupported audio format"));
}

} // namespace

AudioClipAL::AudioClipAL() noexcept = default;

AudioClipAL::~AudioClipAL() noexcept
{
    if (buffer_) {
        alDeleteBuffers(1, &(*buffer_));
        POMDOG_CHECK_ERROR_OPENAL("alDeleteBuffers");
    }
}

std::unique_ptr<Error>
AudioClipAL::initialize(
    const void* data,
    std::size_t sizeInBytesIn,
    int sampleRateIn,
    int bitsPerSampleIn,
    AudioChannels channelsIn) noexcept
{
    sizeInBytes_ = sizeInBytesIn;
    sampleRate_ = sampleRateIn;
    bitsPerSample_ = bitsPerSampleIn;
    channels_ = channelsIn;

    POMDOG_ASSERT(bitsPerSample_ == 8 || bitsPerSample_ == 16);

    buffer_ = [] {
        ALuint nativeBuffer;
        alGenBuffers(1, &nativeBuffer);
        return std::move(nativeBuffer);
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alGenBuffers() failed.");
    }

    POMDOG_ASSERT(buffer_ != std::nullopt);
    POMDOG_ASSERT(data != nullptr);
    POMDOG_ASSERT(sizeInBytes_ > 0);

    auto [format, formatErr] = ToFormat(channels_, bitsPerSample_);
    if (formatErr != nullptr) {
        return errors::wrap(std::move(formatErr), "ToFormat() failed.");
    }

    alBufferData(*buffer_, format, data, static_cast<ALsizei>(sizeInBytes_), sampleRate_);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alBufferData() failed.");
    }

    return nullptr;
}

Duration AudioClipAL::getLength() const noexcept
{
    const auto samples = detail::AudioHelper::getSamples(sizeInBytes_, bitsPerSample_, channels_);
    const auto sampleDuration = detail::AudioHelper::getSampleDuration(samples, sampleRate_);
    return sampleDuration;
}

int AudioClipAL::getSampleRate() const noexcept
{
    return sampleRate_;
}

int AudioClipAL::getBitsPerSample() const noexcept
{
    return bitsPerSample_;
}

AudioChannels AudioClipAL::getChannels() const noexcept
{
    return channels_;
}

std::size_t AudioClipAL::getSizeInBytes() const noexcept
{
    return sizeInBytes_;
}

ALuint AudioClipAL::getNativeBuffer() const noexcept
{
    POMDOG_ASSERT(buffer_ != std::nullopt);
    return *buffer_;
}

} // namespace pomdog::detail::openal
