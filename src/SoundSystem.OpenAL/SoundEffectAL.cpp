// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "SoundEffectAL.hpp"
#include "AudioClipAL.hpp"
#include "ErrorCheckerAL.hpp"
#include "Pomdog/Audio/AudioEmitter.hpp"
#include "Pomdog/Audio/AudioListener.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace OpenAL {

SoundEffectAL::SoundEffectAL(
    AudioEngineAL &,
    const std::shared_ptr<AudioClipAL>& audioClipIn,
    bool isLooped)
    : audioClip(audioClipIn)
{
    source = ([] {
        ALuint nativeSource;
        alGenSources(1, &nativeSource);
        return std::make_optional(nativeSource);
    })();

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alGenSources", __FILE__, __LINE__);
#endif

    POMDOG_ASSERT(source);
    alSourcei(source->value, AL_BUFFER, audioClip->NativeBuffer());

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif

    POMDOG_ASSERT(source);
    alSourcei(source->value, AL_LOOPING, (isLooped ? AL_TRUE: AL_FALSE));

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif
}

SoundEffectAL::~SoundEffectAL()
{
    if (source) {
        alDeleteSources(1, source->Data());
    }
}

void SoundEffectAL::Pause()
{
    POMDOG_ASSERT(source);
    alSourcePause(source->value);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcePause", __FILE__, __LINE__);
#endif
}

void SoundEffectAL::Play()
{
    POMDOG_ASSERT(source);
    alSourcePlay(source->value);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcePlay", __FILE__, __LINE__);
#endif
}

void SoundEffectAL::Stop()
{
    POMDOG_ASSERT(source);
    alSourceStop(source->value);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourceStop", __FILE__, __LINE__);
#endif

    POMDOG_ASSERT(source);
    alSourceRewind(source->value);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourceRewind", __FILE__, __LINE__);
#endif
}

void SoundEffectAL::Apply3D(const AudioListener& listener, const AudioEmitter& emitter)
{
    {
        POMDOG_ASSERT(source);
        alSourcefv(source->value, AL_POSITION, emitter.Position.Data());
        alSourcefv(source->value, AL_VELOCITY, emitter.Velocity.Data());
        alSourcefv(source->value, AL_DIRECTION, emitter.Forward.Data());

#ifdef DEBUG
        ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif

        POMDOG_ASSERT(emitter.DopplerScale >= 0.0f);
        alDopplerFactor(emitter.DopplerScale);

#ifdef DEBUG
        ErrorCheckerAL::CheckError("alDopplerFactor", __FILE__, __LINE__);
#endif
    }
    {
        std::array<ALfloat, 6> orientation {{
            listener.Forward.X, listener.Forward.Y, listener.Forward.Z,
            listener.Up.X, listener.Up.Y, listener.Up.Z}};

        alListenerfv(AL_POSITION, listener.Position.Data());
        alListenerfv(AL_VELOCITY, listener.Velocity.Data());
        alListenerfv(AL_ORIENTATION, orientation.data());

#ifdef DEBUG
        ErrorCheckerAL::CheckError("alListenerfv", __FILE__, __LINE__);
#endif
    }
}

void SoundEffectAL::ExitLoop()
{
    POMDOG_ASSERT(source);
    alSourcei(source->value, AL_LOOPING, AL_FALSE);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif
}

void SoundEffectAL::SetPitch(float pitch)
{
    // pitch --> nativePitch
    //  0.0f --> 1.0f
    // -1.0f --> 0.5f
    // +1.0f --> 1.5f
    auto nativePitch = 1.0f + (0.5f * pitch);

    POMDOG_ASSERT(source);
    POMDOG_ASSERT(nativePitch > 0.0f);
    alSourcef(source->value, AL_PITCH, nativePitch);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif
}

void SoundEffectAL::SetVolume(float volume)
{
    POMDOG_ASSERT(source);
    POMDOG_ASSERT(volume >= 0.0f);
    alSourcef(source->value, AL_GAIN, volume);

#ifdef DEBUG
    ErrorCheckerAL::CheckError("alSourcei", __FILE__, __LINE__);
#endif
}

} // namespace OpenAL
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
