// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Audio/SoundEffect.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/SoundEffectAL.hpp"
#elif defined(POMDOG_PLATFORM_LINUX)
#include "../SoundSystem.OpenAL/SoundEffectAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/SoundEffectXAudio2.hpp"
#endif

#include "Pomdog/Audio/AudioClip.hpp"
#include "Pomdog/Audio/AudioEngine.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace {

static std::shared_ptr<Detail::SoundSystem::NativeAudioClip> GetNativeBuffer(std::shared_ptr<AudioClip> const& audioClip)
{
    std::shared_ptr<Detail::SoundSystem::NativeAudioClip> nativeAudioClip(
        audioClip, audioClip->NativeAudioClip());
    return std::move(nativeAudioClip);
}

} // unnamed namespace
//-----------------------------------------------------------------------
SoundEffect::SoundEffect(AudioEngine & audioEngine,
    std::shared_ptr<AudioClip> const& audioClip, bool isLoopedIn)
    : pitch(0.0f)
    , volume(1.0f)
    , state(SoundState::Stopped)
    , isLooped(isLoopedIn)
{
    auto nativeAudioEngine = audioEngine.NativeAudioEngine();
    POMDOG_ASSERT(nativeAudioEngine);

    nativeSoundEffect = std::make_unique<Detail::SoundSystem::NativeSoundEffect>(
        *nativeAudioEngine, GetNativeBuffer(audioClip), isLooped);
}
//-----------------------------------------------------------------------
SoundEffect::~SoundEffect() = default;
//-----------------------------------------------------------------------
void SoundEffect::Pause()
{
    if (state == SoundState::Paused) {
        return;
    }

    POMDOG_ASSERT(nativeSoundEffect);
    nativeSoundEffect->Pause();
    state = SoundState::Paused;
}
//-----------------------------------------------------------------------
void SoundEffect::Play()
{
    if (state == SoundState::Playing) {
        return;
    }

    POMDOG_ASSERT(nativeSoundEffect);
    nativeSoundEffect->Play();
    state = SoundState::Playing;
}
//-----------------------------------------------------------------------
void SoundEffect::Stop()
{
    if (state == SoundState::Stopped) {
        return;
    }

    POMDOG_ASSERT(nativeSoundEffect);
    nativeSoundEffect->Stop();
    state = SoundState::Stopped;
}
//-----------------------------------------------------------------------
void SoundEffect::Apply3D(AudioListener const& listener, AudioEmitter const& emitter)
{
    POMDOG_ASSERT(nativeSoundEffect);
    nativeSoundEffect->Apply3D(listener, emitter);
}
//-----------------------------------------------------------------------
bool SoundEffect::IsLooped() const
{
    return isLooped;
}
//-----------------------------------------------------------------------
void SoundEffect::ExitLoop()
{
    if (!isLooped) {
        return;
    }

    POMDOG_ASSERT(nativeSoundEffect);
    nativeSoundEffect->ExitLoop();
    isLooped = false;
}
//-----------------------------------------------------------------------
SoundState SoundEffect::State() const
{
    return state;
}
//-----------------------------------------------------------------------
//float SoundEffect::Pan() const
//{
//}
//-----------------------------------------------------------------------
//void SoundEffect::Pan(float panpot)
//{
//    //POMDOG_ASSERT(nativeSoundEffect);
//    //nativeSoundEffect->Pan(panpot);
//}
//-----------------------------------------------------------------------
float SoundEffect::Pitch() const
{
    return pitch;
}
//-----------------------------------------------------------------------
void SoundEffect::Pitch(float pitchIn)
{
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);
    POMDOG_ASSERT(nativeSoundEffect);
    this->pitch = pitchIn;
    nativeSoundEffect->Pitch(pitchIn);
}
//-----------------------------------------------------------------------
float SoundEffect::Volume() const
{
    return volume;
}
//-----------------------------------------------------------------------
void SoundEffect::Volume(float volumeIn)
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);
    POMDOG_ASSERT(nativeSoundEffect);
    this->volume = volumeIn;
    nativeSoundEffect->Volume(volume);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
