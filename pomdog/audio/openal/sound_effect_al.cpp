// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/openal/sound_effect_al.h"
#include "pomdog/audio/audio_emitter.h"
#include "pomdog/audio/audio_listener.h"
#include "pomdog/audio/openal/audio_clip_al.h"
#include "pomdog/audio/openal/error_checker_al.h"
#include "pomdog/math/math.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include <array>
#include <cmath>
#include <utility>

namespace pomdog::detail::openal {

SoundEffectAL::SoundEffectAL() noexcept = default;

SoundEffectAL::~SoundEffectAL() noexcept
{
    if (source != std::nullopt) {
        alDeleteSources(1, &*source);
        source = std::nullopt;
    }
}

std::unique_ptr<Error>
SoundEffectAL::Initialize(
    const std::shared_ptr<AudioClipAL>& audioClipIn,
    bool isLoopedIn) noexcept
{
    this->audioClip = audioClipIn;
    this->isLooped = isLoopedIn;

    source = [] {
        ALuint nativeSource;
        alGenSources(1, &nativeSource);
        return std::make_optional(nativeSource);
    }();
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alGenSources() failed.");
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourcei(source.value(), AL_BUFFER, static_cast<ALint>(audioClip->GetNativeBuffer()));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alSourcei(AL_BUFFER) failed.");
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourcei(source.value(), AL_LOOPING, (isLooped ? AL_TRUE : AL_FALSE));
    if (auto err = alGetError(); err != AL_NO_ERROR) {
        return MakeOpenALError(std::move(err), "alSourcei(AL_LOOPING) failed.");
    }

    return nullptr;
}

void SoundEffectAL::Pause() noexcept
{
    if (this->state == SoundState::Paused) {
        return;
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourcePause(source.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourcePause()");
    this->state = SoundState::Paused;
}

void SoundEffectAL::Play() noexcept
{
    if (this->state == SoundState::Playing) {
        return;
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourcePlay(source.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourcePlay()");
    this->state = SoundState::Playing;
}

void SoundEffectAL::Stop() noexcept
{
    if (this->state == SoundState::Stopped) {
        return;
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourceStop(source.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourceStop()");

    POMDOG_ASSERT(source != std::nullopt);
    alSourceRewind(source.value());
    POMDOG_CHECK_ERROR_OPENAL("alSourceRewind()");
    this->state = SoundState::Stopped;
}

void SoundEffectAL::Apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept
{
    {
        POMDOG_ASSERT(source != std::nullopt);
        alSourcefv(source.value(), AL_POSITION, emitter.Position.Data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_POSITION)");

        alSourcefv(source.value(), AL_VELOCITY, emitter.Velocity.Data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_VELOCITY)");

        alSourcefv(source.value(), AL_DIRECTION, emitter.Forward.Data());
        POMDOG_CHECK_ERROR_OPENAL("alSourcefv(AL_DIRECTION)");

        POMDOG_ASSERT(emitter.DopplerScale >= 0.0f);
        alDopplerFactor(emitter.DopplerScale);
        POMDOG_CHECK_ERROR_OPENAL("alDopplerFactor()");
    }
    {
        std::array<ALfloat, 6> orientation{{
            listener.Forward.x,
            listener.Forward.y,
            listener.Forward.z,
            listener.Up.x,
            listener.Up.y,
            listener.Up.z,
        }};

        alListenerfv(AL_POSITION, listener.Position.Data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_POSITION)");

        alListenerfv(AL_VELOCITY, listener.Velocity.Data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_VELOCITY)");

        alListenerfv(AL_ORIENTATION, orientation.data());
        POMDOG_CHECK_ERROR_OPENAL("alListenerfv(AL_ORIENTATION)");
    }
}

bool SoundEffectAL::IsLooped() const noexcept
{
    return this->isLooped;
}

void SoundEffectAL::ExitLoop() noexcept
{
    if (!this->isLooped) {
        return;
    }

    POMDOG_ASSERT(source != std::nullopt);
    alSourcei(source.value(), AL_LOOPING, AL_FALSE);
    POMDOG_CHECK_ERROR_OPENAL("alSourcei()");
    this->isLooped = false;
}

SoundState SoundEffectAL::GetState() const noexcept
{
    return state;
}

float SoundEffectAL::GetPitch() const noexcept
{
    return pitch;
}

void SoundEffectAL::SetPitch(float pitchIn) noexcept
{
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);
    this->pitch = pitchIn;

    // NOTE: Convert from pitch to nativePitch:
    //  0.0f => 1.0f
    // -1.0f => 0.5f
    // +1.0f => 1.5f
    const auto nativePitch = 1.0f + (0.5f * pitch);

    POMDOG_ASSERT(source != std::nullopt);
    POMDOG_ASSERT(nativePitch > 0.0f);
    alSourcef(source.value(), AL_PITCH, nativePitch);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

float SoundEffectAL::GetVolume() const noexcept
{
    return volume;
}

void SoundEffectAL::SetVolume(float volumeIn) noexcept
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);
    this->volume = volumeIn;

    POMDOG_ASSERT(source != std::nullopt);
    POMDOG_ASSERT(volume >= 0.0f);
    alSourcef(source.value(), AL_GAIN, volume);
    POMDOG_CHECK_ERROR_OPENAL("alSourcef()");
}

} // namespace pomdog::detail::openal
