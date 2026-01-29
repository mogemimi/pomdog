// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/sound_effect_al.h"
#include "pomdog/audio/audio_emitter.h"
#include "pomdog/audio/audio_listener.h"
#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::openal {

SoundEffectAL::SoundEffectAL() noexcept = default;

SoundEffectAL::~SoundEffectAL() noexcept
{
    if (source_ != std::nullopt) {
        alDeleteSources(1, &*source_);
        source_ = std::nullopt;
    }
}

std::unique_ptr<Error>
SoundEffectAL::initialize(
    const std::shared_ptr<AudioClipAL>& audioClipIn,
    bool isLoopedIn) noexcept
{
    audioClip_ = audioClipIn;
    isLooped_ = isLoopedIn;

    source_ = [] {
        ALuint nativeSource;
        alGenSources(1, &nativeSource);
        return std::make_optional(nativeSource);
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alGenSources() failed.");
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcei(source_.value(), AL_BUFFER, static_cast<ALint>(audioClip_->getNativeBuffer()));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alSourcei(AL_BUFFER) failed.");
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcei(source_.value(), AL_LOOPING, (isLooped_ ? AL_TRUE : AL_FALSE));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alSourcei(AL_LOOPING) failed.");
    }

    return nullptr;
}

void SoundEffectAL::pause() noexcept
{
    if (state_ == SoundState::Paused) {
        return;
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePause(source_.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourcePause()");
    state_ = SoundState::Paused;
}

void SoundEffectAL::play() noexcept
{
    if (state_ == SoundState::Playing) {
        return;
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcePlay(source_.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourcePlay()");
    state_ = SoundState::Playing;
}

void SoundEffectAL::stop() noexcept
{
    if (state_ == SoundState::Stopped) {
        return;
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourceStop(source_.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourceStop()");

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourceRewind(source_.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourceRewind()");
    state_ = SoundState::Stopped;
}

void SoundEffectAL::apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept
{
    {
        POMDOG_ASSERT(source_ != std::nullopt);
        alSourcefv(source_.value(), AL_POSITION, emitter.position.data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_POSITION)");

        alSourcefv(source_.value(), AL_VELOCITY, emitter.velocity.data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_VELOCITY)");

        alSourcefv(source_.value(), AL_DIRECTION, emitter.forward.data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_DIRECTION)");

        POMDOG_ASSERT(emitter.dopplerScale >= 0.0f);
        alDopplerFactor(emitter.dopplerScale);
        POMDOG_CHECK_ERROR_OPENAL("alDopplerFactor()");
    }
    {
        std::array<ALfloat, 6> orientation{{
            listener.forward.x,
            listener.forward.y,
            listener.forward.z,
            listener.up.x,
            listener.up.y,
            listener.up.z,
        }};

        alListenerfv(AL_POSITION, listener.position.data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_POSITION)");

        alListenerfv(AL_VELOCITY, listener.velocity.data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_VELOCITY)");

        alListenerfv(AL_ORIENTATION, orientation.data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_ORIENTATION)");
    }
}

bool SoundEffectAL::isLooped() const noexcept
{
    return isLooped_;
}

void SoundEffectAL::exitLoop() noexcept
{
    if (!isLooped_) {
        return;
    }

    POMDOG_ASSERT(source_ != std::nullopt);
    alSourcei(source_.value(), AL_LOOPING, AL_FALSE);
    POMDOG_CHECK_ERROR_OPENAL("alSourcei()");
    isLooped_ = false;
}

SoundState SoundEffectAL::getState() const noexcept
{
    return state_;
}

float SoundEffectAL::getPitch() const noexcept
{
    return pitch_;
}

void SoundEffectAL::setPitch(float pitchIn) noexcept
{
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);
    pitch_ = pitchIn;

    // NOTE: Convert from pitch to nativePitch:
    //  0.0f => 1.0f
    // -1.0f => 0.5f
    // +1.0f => 1.5f
    const auto nativePitch = 1.0f + (0.5f * pitch_);

    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(nativePitch > 0.0f);
    alSourcef(source_.value(), AL_PITCH, nativePitch);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

float SoundEffectAL::getVolume() const noexcept
{
    return volume_;
}

void SoundEffectAL::setVolume(float volumeIn) noexcept
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);
    volume_ = volumeIn;

    POMDOG_ASSERT(source_ != std::nullopt);
    POMDOG_ASSERT(volume_ >= 0.0f);
    alSourcef(source_.value(), AL_GAIN, volume_);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

} // namespace pomdog::detail::openal
