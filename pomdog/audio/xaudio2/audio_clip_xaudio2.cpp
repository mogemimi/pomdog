// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {

AudioClipXAudio2::AudioClipXAudio2() noexcept = default;

AudioClipXAudio2::~AudioClipXAudio2() noexcept = default;

std::unique_ptr<Error>
AudioClipXAudio2::initialize(
    std::span<const u8> audioData,
    i32 sampleRate,
    i32 bitsPerSample,
    AudioChannels channels) noexcept
{
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
    if ((bitsPerSample != 8) && (bitsPerSample != 16) && (bitsPerSample != 24) && (bitsPerSample != 32)) {
        return errors::make("unsupported bitsPerSample");
    }
    if ((channels != AudioChannels::Mono) && (channels != AudioChannels::Stereo)) {
        return errors::make("unsupported audio channels");
    }

    channels_ = channels;
    sizeInBytes_ = static_cast<i32>(audioData.size());
    bitsPerSample_ = bitsPerSample;
    samplesPerSec_ = sampleRate;
    sampleCount_ = detail::calculateSampleCount(sizeInBytes_, bitsPerSample, channels);
    sampleDuration_ = detail::calculateSampleDuration(sampleCount_, samplesPerSec_);

    const auto audioDataSize = audioData.size();
    audioData_ = std::make_unique<u8[]>(audioDataSize);
    std::memcpy(audioData_.get(), audioData.data(), audioDataSize);

    return nullptr;
}

Duration
AudioClipXAudio2::getLength() const noexcept
{
    return sampleDuration_;
}

i32 AudioClipXAudio2::getSampleCount() const noexcept
{
    return sampleCount_;
}

i32 AudioClipXAudio2::getSampleRate() const noexcept
{
    return samplesPerSec_;
}

i32 AudioClipXAudio2::getBitsPerSample() const noexcept
{
    return bitsPerSample_;
}

AudioChannels
AudioClipXAudio2::getChannels() const noexcept
{
    return channels_;
}

bool AudioClipXAudio2::isStreamable() const noexcept
{
    return false;
}

unsafe_ptr<const u8>
AudioClipXAudio2::getData() const noexcept
{
    return audioData_.get();
}

i32 AudioClipXAudio2::getSizeInBytes() const noexcept
{
    return sizeInBytes_;
}

} // namespace pomdog::detail::xaudio2
