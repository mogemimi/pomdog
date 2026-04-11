// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_source_streaming_al.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/audio/openal/audio_clip_streaming_al.h"
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

AudioSourceStreamingAL::AudioSourceStreamingAL() noexcept = default;

AudioSourceStreamingAL::~AudioSourceStreamingAL() noexcept
{
    if (source_ != std::nullopt) {
        alDeleteSources(1, &(*source_));
        POMDOG_CHECK_ERROR_OPENAL("alDeleteSources()");
        source_ = std::nullopt;
    }

    for (auto& buffer : buffers_) {
        alDeleteBuffers(1, &buffer);
        POMDOG_CHECK_ERROR_OPENAL("alDeleteBuffers");
    }
}

std::unique_ptr<Error>
AudioSourceStreamingAL::initialize(
    std::shared_ptr<AudioClipStreamingAL> audioClip,
    std::span<u8> tempBuffer,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(source_ == std::nullopt);

    audioClip_ = std::move(audioClip);
    pitch_ = 0.0f;
    volume_ = 1.0f;
    queueSchedule_ = QueueScheduleKind::CanEnqueue;

    // NOTE: Do not set a loop in alSourcei(AL_LOOPING) when streaming.
    isLooped_ = isLooped;

    source_ = []() -> ALuint {
        ALuint nativeSource;
        alGenSources(1, &nativeSource);
        return nativeSource;
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alGenSources() failed");
    }

    POMDOG_ASSERT(source_ != std::nullopt);

    alSourcef(*source_, AL_GAIN, volume_);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alSourcef(AL_GAIN) failed");
    }

    alGenBuffers(static_cast<ALsizei>(buffers_.size()), buffers_.data());
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alGenBuffers() failed");
    }

    const auto samplesPerSec = audioClip_->getSampleRate();
    const auto bitsPerSample = audioClip_->getBitsPerSample();
    const auto channels = audioClip_->getChannels();

    POMDOG_ASSERT(samplesPerSec > 0);
    POMDOG_ASSERT(bitsPerSample >= 8);

    const auto format = toFormat(channels, bitsPerSample);
    if (format == std::nullopt) {
        return errors::make("unsupported audio format");
    }

    if (tempBuffer.empty()) {
        return errors::make("tempBuffer is empty");
    }

    if (auto err = audioClip_->seekStart(); err != nullptr) {
        return errors::wrap(std::move(err), "seekStart() failed");
    }

    bool canQueueBuffer = true;

    for (auto& buffer : buffers_) {
        std::span<u8> bufferView{tempBuffer};

        auto [sampleCount, sampleErr] = audioClip_->readSamplesInterleaved(bufferView, isLooped_);
        if (sampleErr != nullptr) {
            return errors::wrap(std::move(sampleErr), "readSamplesInterleaved() failed");
        }

        if (sampleCount <= 0) {
            if (auto err = audioClip_->seekStart(); err != nullptr) {
                return errors::wrap(std::move(err), "seekStart() failed");
            }

            if (!isLooped_) {
                canQueueBuffer = false;
                queueSchedule_ = QueueScheduleKind::DequeueOnly;
            }

            std::tie(sampleCount, sampleErr) = audioClip_->readSamplesInterleaved(bufferView, isLooped_);
            if (sampleErr != nullptr) {
                return errors::wrap(std::move(sampleErr), "readSamplesInterleaved() failed");
            }
            if (sampleCount <= 0) {
                break;
            }
        }
        bufferView = bufferView.first(static_cast<u32>((bitsPerSample / 8) * detail::toAudioChannelCount(channels) * sampleCount));

        alBufferData(buffer, *format, bufferView.data(), static_cast<ALsizei>(bufferView.size()), samplesPerSec);
        if (auto err = alGetError(); err != AL_NO_ERROR) {
            return makeOpenALError(std::move(err), "alBufferData() failed.");
        }

        if (canQueueBuffer) {
            alSourceQueueBuffers(*source_, 1, &buffer);
            if (auto err = alGetError(); err != AL_NO_ERROR) {
                return makeOpenALError(std::move(err), "alSourceQueueBuffers() failed.");
            }
        }
        else {
            POMDOG_ASSERT(waitBuffers_.size() < waitBuffers_.capacity());
            waitBuffers_.push_back(buffer);
        }
    }

    return nullptr;
}

std::unique_ptr<Error>
AudioSourceStreamingAL::poll(std::span<u8> tempBuffer) noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(audioClip_ != nullptr);

    if (queueSchedule_ == QueueScheduleKind::Stop) {
        return nullptr;
    }

    const auto samplesPerSec = audioClip_->getSampleRate();
    const auto bitsPerSample = audioClip_->getBitsPerSample();
    const auto channels = audioClip_->getChannels();

    POMDOG_ASSERT(samplesPerSec > 0);
    POMDOG_ASSERT(bitsPerSample >= 8);

    const auto format = toFormat(channels, bitsPerSample);
    if (format == std::nullopt) {
        return errors::make("unsupported audio format");
    }

    bool canQueueBuffer = false;

    switch (queueSchedule_) {
    case QueueScheduleKind::CanEnqueue: {
        canQueueBuffer = true;
        break;
    }
    case QueueScheduleKind::DequeueOnly: {
        break;
    }
    case QueueScheduleKind::ExitLoop: {
        ALenum state = AL_STOPPED;
        alGetSourcei(*source_, AL_SOURCE_STATE, &state);
        POMDOG_CHECK_ERROR_OPENAL("alGetSourcei(AL_SOURCE_STATE)");
        if (state == AL_PLAYING) {
            ALint queued = 0;
            alGetSourcei(*source_, AL_BUFFERS_QUEUED, &queued);
            POMDOG_CHECK_ERROR_OPENAL("alGetSourcei()");

            ALint processed = 0;
            alGetSourcei(*source_, AL_BUFFERS_PROCESSED, &processed);
            POMDOG_CHECK_ERROR_OPENAL("alGetSourcei()");

            if ((queued == 0) && (processed == 0)) {
                queueSchedule_ = QueueScheduleKind::Stop;
            }
        }
        else {
            queueSchedule_ = QueueScheduleKind::Stop;
        }
        break;
    }
    case QueueScheduleKind::Stop: {
        break;
    }
    }

    if (canQueueBuffer && !waitBuffers_.empty()) {
        for (auto& buffer : waitBuffers_) {
            alSourceQueueBuffers(*source_, 1, &buffer);
            POMDOG_CHECK_ERROR_OPENAL("alSourceQueueBuffers()");
        }
        waitBuffers_.clear();
    }

    ALint processed = 0;
    alGetSourcei(*source_, AL_BUFFERS_PROCESSED, &processed);
    POMDOG_CHECK_ERROR_OPENAL("alGetSourcei()");

    POMDOG_ASSERT(processed <= static_cast<ALint>(buffers_.size()));

    bool queued = false;

    while (processed > 0) {
        ALuint buffer = 0;
        alSourceUnqueueBuffers(*source_, 1, &buffer);
        POMDOG_CHECK_ERROR_OPENAL("alSourceUnqueueBuffers()");

        --processed;

        std::span<u8> bufferView{tempBuffer};

        auto [sampleCount, sampleErr] = audioClip_->readSamplesInterleaved(bufferView, isLooped_);
        if (sampleErr != nullptr) {
            return errors::wrap(std::move(sampleErr), "readSamplesInterleaved() failed");
        }

        if (sampleCount <= 0) {
            if (auto err = audioClip_->seekStart(); err != nullptr) {
                return errors::wrap(std::move(err), "seekStart() failed");
            }

            if (!isLooped_) {
                canQueueBuffer = false;
                queueSchedule_ = QueueScheduleKind::ExitLoop;
            }

            std::tie(sampleCount, sampleErr) = audioClip_->readSamplesInterleaved(bufferView, isLooped_);
            if (sampleErr != nullptr) {
                return errors::wrap(std::move(sampleErr), "readSamplesInterleaved() failed");
            }
            if (sampleCount <= 0) {
                // NOTE: reuse buffer
                POMDOG_ASSERT(waitBuffers_.size() < waitBuffers_.capacity());
                waitBuffers_.push_back(buffer);
                continue;
            }
        }
        bufferView = bufferView.first(static_cast<u32>((bitsPerSample / 8) * detail::toAudioChannelCount(channels) * sampleCount));

        alBufferData(buffer, *format, bufferView.data(), static_cast<ALsizei>(bufferView.size()), samplesPerSec);
        POMDOG_CHECK_ERROR_OPENAL("alBufferData()");

        if (canQueueBuffer) {
            alSourceQueueBuffers(*source_, 1, &buffer);
            POMDOG_CHECK_ERROR_OPENAL("alSourceQueueBuffers()");

            queued = true;
        }
        else {
            POMDOG_ASSERT(waitBuffers_.size() < waitBuffers_.capacity());
            waitBuffers_.push_back(buffer);
        }
    }

    if (queued) {
        ALenum state = AL_STOPPED;
        alGetSourcei(*source_, AL_SOURCE_STATE, &state);
        POMDOG_CHECK_ERROR_OPENAL("alGetSourcei(AL_SOURCE_STATE)");
        if (state != AL_PLAYING) {
            alSourcePlay(*source_);
        }
    }

    if (queueSchedule_ == QueueScheduleKind::Stop) {
        alSourceStop(*source_);
        POMDOG_CHECK_ERROR_OPENAL("alSourceStop()");

        if (auto err = audioClip_->seekStart(); err != nullptr) {
            return errors::wrap(std::move(err), "seekStart() failed");
        }
    }

    return nullptr;
}

void AudioSourceStreamingAL::pause() noexcept
{
    queueSchedule_ = QueueScheduleKind::DequeueOnly;

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePause(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourcePause()");
}

void AudioSourceStreamingAL::play() noexcept
{
    queueSchedule_ = QueueScheduleKind::CanEnqueue;

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePlay(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourcePlay()");
}

void AudioSourceStreamingAL::stop() noexcept
{
    queueSchedule_ = QueueScheduleKind::Stop;

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourceStop(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourceStop()");

    POMDOG_ASSERT(audioClip_ != nullptr);
    if (auto err = audioClip_->seekStart(); err != nullptr) {
        // TODO: error handling
    }
}

bool AudioSourceStreamingAL::isLooped() const noexcept
{
    return isLooped_;
}

void AudioSourceStreamingAL::exitLoop() noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);

    if (!isLooped_) {
        return;
    }

    isLooped_ = false;
}

bool AudioSourceStreamingAL::isPlaying() const noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);

    ALenum state = AL_STOPPED;
    alGetSourcei(*source_, AL_SOURCE_STATE, &state);
    POMDOG_CHECK_ERROR_OPENAL("alGetSourcei(AL_SOURCE_STATE)");
    return (state == AL_PLAYING);
}

f32 AudioSourceStreamingAL::getPitch() const noexcept
{
    return pitch_;
}

void AudioSourceStreamingAL::setPitch(f32 pitch) noexcept
{
    POMDOG_ASSERT(pitch >= -1.0f);
    POMDOG_ASSERT(pitch <= 1.0f);
    pitch_ = pitch;

    const auto nativePitch = 1.0f + (0.5f * pitch);

    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(nativePitch > 0.0f);
    alSourcef(*source_, AL_PITCH, nativePitch);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

f32 AudioSourceStreamingAL::getVolume() const noexcept
{
    return volume_;
}

void AudioSourceStreamingAL::setVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(volume >= 0.0f);
    POMDOG_ASSERT(volume <= 1.0f);

    volume_ = volume;
    alSourcef(*source_, AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

} // namespace pomdog::detail::openal
