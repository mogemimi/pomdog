// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

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

std::shared_ptr<Detail::SoundSystem::NativeAudioClip>
GetNativeBuffer(std::shared_ptr<AudioClip> const& audioClip)
{
    std::shared_ptr<Detail::SoundSystem::NativeAudioClip> nativeAudioClip(
        audioClip, audioClip->GetNativeAudioClip());
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
    auto nativeAudioEngine = audioEngine.GetNativeAudioEngine();
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
SoundState SoundEffect::GetState() const
{
    return state;
}
//-----------------------------------------------------------------------
//float SoundEffect::GetPan() const
//{
//}
//-----------------------------------------------------------------------
//void SoundEffect::SetPan(float panpot)
//{
//    //POMDOG_ASSERT(nativeSoundEffect);
//    //nativeSoundEffect->Pan(panpot);
//}
//-----------------------------------------------------------------------
float SoundEffect::GetPitch() const
{
    return pitch;
}
//-----------------------------------------------------------------------
void SoundEffect::SetPitch(float pitchIn)
{
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);
    POMDOG_ASSERT(nativeSoundEffect);
    this->pitch = pitchIn;
    nativeSoundEffect->SetPitch(pitchIn);
}
//-----------------------------------------------------------------------
float SoundEffect::GetVolume() const
{
    return volume;
}
//-----------------------------------------------------------------------
void SoundEffect::SetVolume(float volumeIn)
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);
    POMDOG_ASSERT(nativeSoundEffect);
    this->volume = volumeIn;
    nativeSoundEffect->SetVolume(volume);
}
//-----------------------------------------------------------------------
} // namespace Pomdog
