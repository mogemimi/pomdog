// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_clip_streaming_al.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstring>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::openal {

AudioClipStreamingAL::AudioClipStreamingAL() noexcept = default;

AudioClipStreamingAL::~AudioClipStreamingAL() noexcept = default;

std::unique_ptr<Error>
AudioClipStreamingAL::initialize(std::unique_ptr<detail::AudioClipFile> audioFile) noexcept
{
    audioFile_ = std::move(audioFile);

    const auto channels = audioFile_->getChannels();
    const auto bitsPerSample = audioFile_->getBitsPerSample();
    const auto samplesPerSec = audioFile_->getSampleRate();
    const auto sampleCount = audioFile_->getSampleCount();
    sampleDuration_ = calculateSampleDuration(sampleCount, samplesPerSec);

    if (sampleCount <= 0) {
        return errors::make("sampleCount must be > 0");
    }
    if (samplesPerSec <= 0) {
        return errors::make("samplesPerSec must be > 0");
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

    return nullptr;
}

Duration AudioClipStreamingAL::getLength() const noexcept
{
    return sampleDuration_;
}

i32 AudioClipStreamingAL::getSampleCount() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getSampleCount();
}

i32 AudioClipStreamingAL::getSampleRate() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getSampleRate();
}

i32 AudioClipStreamingAL::getBitsPerSample() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getBitsPerSample();
}

AudioChannels AudioClipStreamingAL::getChannels() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getChannels();
}

bool AudioClipStreamingAL::isStreamable() const noexcept
{
    return true;
}

std::unique_ptr<Error>
AudioClipStreamingAL::seekStart()
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->seekStart();
}

std::tuple<i32, std::unique_ptr<Error>>
AudioClipStreamingAL::readSamplesInterleaved(std::span<u8> outBuffer, bool looping)
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    auto [samples, err] = audioFile_->readSamplesInterleaved(outBuffer);
    if (err != nullptr) {
        return std::make_tuple(samples, std::move(err));
    }

    const auto channels = audioFile_->getChannels();
    const auto bitsPerSample = audioFile_->getBitsPerSample();

    auto readSize = static_cast<u32>((bitsPerSample / 8) * toAudioChannelCount(channels) * samples);
    if (readSize >= outBuffer.size()) {
        return std::make_tuple(samples, nullptr);
    }

    if (!looping) {
        auto subspan = outBuffer.subspan(readSize);
        if (!subspan.empty()) {
            std::memset(subspan.data(), 0x00, subspan.size());
        }
        return std::make_tuple(samples, nullptr);
    }

    auto subspan = outBuffer.subspan(readSize);
    POMDOG_ASSERT(!subspan.empty());

    err = audioFile_->seekStart();
    if (err != nullptr) {
        return std::make_tuple(samples, std::move(err));
    }

    auto [subSamples, subErr] = audioFile_->readSamplesInterleaved(subspan);
    if (subErr != nullptr) {
        return std::make_tuple(samples, std::move(subErr));
    }
    samples += subSamples;

    return std::make_tuple(samples, nullptr);
}

} // namespace pomdog::detail::openal
