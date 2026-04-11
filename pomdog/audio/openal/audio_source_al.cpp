// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/audio_source_al.h"
#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::openal {

AudioSourceAL::AudioSourceAL() noexcept = default;

AudioSourceAL::~AudioSourceAL() noexcept
{
    if (source_ != std::nullopt) {
        alDeleteSources(1, &(*source_));
        POMDOG_CHECK_ERROR_OPENAL("alDeleteSources()");
        source_ = std::nullopt;
    }
}

std::unique_ptr<Error>
AudioSourceAL::initialize(
    std::shared_ptr<AudioClipAL> audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(source_ == std::nullopt);

    audioClip_ = std::move(audioClip);
    pitch_ = 0.0f;
    volume_ = 1.0f;
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
    alSourcei(*source_, AL_BUFFER, static_cast<ALint>(audioClip_->getNativeBuffer()));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alSourcei(AL_BUFFER) failed");
    }
    alSourcei(*source_, AL_LOOPING, (isLooped ? AL_TRUE : AL_FALSE));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alSourcei(AL_LOOPING) failed");
    }
    alSourcef(*source_, AL_GAIN, volume_);
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return makeOpenALError(std::move(err), "alSourcef(AL_GAIN) failed");
    }

    return nullptr;
}

void AudioSourceAL::pause() noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePause(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourcePause()");
}

void AudioSourceAL::play() noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePlay(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourcePlay()");
}

void AudioSourceAL::stop() noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    alSourceStop(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourceStop()");

    alSourceRewind(*source_);
    POMDOG_CHECK_ERROR_OPENAL("alSourceRewind()");
}

bool AudioSourceAL::isLooped() const noexcept
{
    return isLooped_;
}

void AudioSourceAL::exitLoop() noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);

    if (!isLooped_) {
        return;
    }

    alSourcei(*source_, AL_LOOPING, AL_FALSE);
    POMDOG_CHECK_ERROR_OPENAL("alSourcei()");
    isLooped_ = false;
}

bool AudioSourceAL::isPlaying() const noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);

    ALenum state = AL_STOPPED;
    alGetSourcei(*source_, AL_SOURCE_STATE, &state);
    POMDOG_CHECK_ERROR_OPENAL("alGetSourcei(AL_SOURCE_STATE)");
    return (state == AL_PLAYING);
}

f32 AudioSourceAL::getPitch() const noexcept
{
    return pitch_;
}

void AudioSourceAL::setPitch(f32 pitch) noexcept
{
    POMDOG_ASSERT(pitch >= -1.0f);
    POMDOG_ASSERT(pitch <= 1.0f);
    pitch_ = pitch;

    // NOTE: Convert from pitch to nativePitch:
    //        0.0f => 1.0f
    //       -1.0f => 0.5f
    //       +1.0f => 1.5f
    const auto nativePitch = 1.0f + (0.5f * pitch);

    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(nativePitch > 0.0f);
    alSourcef(*source_, AL_PITCH, nativePitch);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

f32 AudioSourceAL::getVolume() const noexcept
{
    return volume_;
}

void AudioSourceAL::setVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(volume >= 0.0f);
    POMDOG_ASSERT(volume <= 1.0f);

    volume_ = volume;
    alSourcef(*source_, AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

} // namespace pomdog::detail::openal
