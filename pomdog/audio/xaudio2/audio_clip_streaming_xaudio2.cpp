// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_clip_streaming_xaudio2.h"
#include "pomdog/audio/details/audio_clip_file.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {

AudioClipStreamingXAudio2::AudioClipStreamingXAudio2() noexcept = default;

AudioClipStreamingXAudio2::~AudioClipStreamingXAudio2() noexcept = default;

std::unique_ptr<Error>
AudioClipStreamingXAudio2::initialize(std::unique_ptr<detail::AudioClipFile> audioFile) noexcept
{
    audioFile_ = std::move(audioFile);

    const auto channels = audioFile_->getChannels();
    const auto bitsPerSample = audioFile_->getBitsPerSample();
    const auto samplesPerSec = audioFile_->getSampleRate();
    const auto sampleCount = audioFile_->getSampleCount();
    sampleDuration_ = detail::calculateSampleDuration(sampleCount, samplesPerSec);

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

Duration AudioClipStreamingXAudio2::getLength() const noexcept
{
    return sampleDuration_;
}

i32 AudioClipStreamingXAudio2::getSampleCount() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getSampleCount();
}

i32 AudioClipStreamingXAudio2::getSampleRate() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getSampleRate();
}

i32 AudioClipStreamingXAudio2::getBitsPerSample() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getBitsPerSample();
}

AudioChannels AudioClipStreamingXAudio2::getChannels() const noexcept
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->getChannels();
}

bool AudioClipStreamingXAudio2::isStreamable() const noexcept
{
    return true;
}

std::unique_ptr<Error>
AudioClipStreamingXAudio2::seekStart()
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    return audioFile_->seekStart();
}

std::tuple<i32, std::unique_ptr<Error>>
AudioClipStreamingXAudio2::readSamplesInterleaved(std::span<u8> outBuffer, bool looping)
{
    POMDOG_ASSERT(audioFile_ != nullptr);
    auto [samples, err] = audioFile_->readSamplesInterleaved(outBuffer);
    if (err != nullptr) {
        return std::make_tuple(samples, std::move(err));
    }

    const auto channels = audioFile_->getChannels();
    const auto bitsPerSample = audioFile_->getBitsPerSample();

    auto readSize = static_cast<u32>((bitsPerSample / 8) * detail::toAudioChannelCount(channels) * samples);
    if (readSize >= outBuffer.size()) {
        return std::make_tuple(samples, nullptr);
    }

    if (!looping) {
        auto subspan = outBuffer.subspan(readSize);
        if (!subspan.empty()) {
            memsetSpan(subspan, u8(0x00));
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

} // namespace pomdog::detail::xaudio2
