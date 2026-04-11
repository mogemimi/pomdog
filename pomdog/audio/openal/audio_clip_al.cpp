// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::openal {
namespace {

[[nodiscard]] std::optional<ALenum>
toFormat(AudioChannels channel, i32 bitPerSample) noexcept
{
    switch (channel) {
    case AudioChannels::Mono:
        if (bitPerSample == 8) {
            return AL_FORMAT_MONO8;
        }
        if (bitPerSample == 16) {
            return AL_FORMAT_MONO16;
        }
        break;
    case AudioChannels::Stereo:
        if (bitPerSample == 8) {
            return AL_FORMAT_STEREO8;
        }
        if (bitPerSample == 16) {
            return AL_FORMAT_STEREO16;
        }
        break;
    }
    return std::nullopt;
}

} // namespace

AudioClipAL::AudioClipAL() noexcept = default;

AudioClipAL::~AudioClipAL() noexcept
{
    if (buffer_ != std::nullopt) {
        alDeleteBuffers(1, &(*buffer_));
        POMDOG_CHECK_ERROR_OPENAL("alDeleteBuffers");
        buffer_ = std::nullopt;
    }
}

std::unique_ptr<Error>
AudioClipAL::initialize(
    std::span<const u8> audioData,
    i32 sampleRate,
    i32 bitsPerSample,
    AudioChannels channels) noexcept
{
    POMDOG_ASSERT(buffer_ == std::nullopt);

    if (audioData.data() == nullptr) {
        return errors::make("audioData.data() must be != nullptr");
    }
    if (audioData.empty()) {
        return errors::make("audioData is empty");
    }
    if (sampleRate <= 0) {
        return errors::make("sampleRate must be > 0");
    }
    if (bitsPerSample < 8) {
        return errors::make("bitsPerSample must be >= 8");
    }

    channels_ = channels;
    bitsPerSample_ = bitsPerSample;
    samplesPerSec_ = sampleRate;
    sampleCount_ = calculateSampleCount(static_cast<i32>(audioData.size()), bitsPerSample, channels);
    sampleDuration_ = calculateSampleDuration(sampleCount_, samplesPerSec_);

    buffer_ = []() -> ALuint {
        ALuint nativeBuffer;
        alGenBuffers(1, &nativeBuffer);
        return nativeBuffer;
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alGenBuffers() failed");
    }

    const auto format = toFormat(channels, bitsPerSample);
    if (format == std::nullopt) {
        return errors::make("unsupported audio format");
    }

    alBufferData(*buffer_, *format, audioData.data(), static_cast<ALsizei>(audioData.size()), sampleRate);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alBufferData() failed");
    }

    return nullptr;
}

Duration AudioClipAL::getLength() const noexcept
{
    return sampleDuration_;
}

i32 AudioClipAL::getSampleCount() const noexcept
{
    return sampleCount_;
}

i32 AudioClipAL::getSampleRate() const noexcept
{
    return samplesPerSec_;
}

i32 AudioClipAL::getBitsPerSample() const noexcept
{
    return bitsPerSample_;
}

AudioChannels AudioClipAL::getChannels() const noexcept
{
    return channels_;
}

bool AudioClipAL::isStreamable() const noexcept
{
    return false;
}

ALuint AudioClipAL::getNativeBuffer() const noexcept
{
    POMDOG_ASSERT(buffer_ != std::nullopt);
    return *buffer_;
}

} // namespace pomdog::detail::openal
