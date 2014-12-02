//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "SoundEffect.hpp"

#if defined(POMDOG_PLATFORM_MACOSX) || defined(POMDOG_PLATFORM_APPLE_IOS)
#include "../SoundSystem.OpenAL/SoundEffectAL.hpp"
#elif defined(POMDOG_PLATFORM_WIN32) || defined(POMDOG_PLATFORM_XBOX_ONE)
#include "../SoundSystem.XAudio2/SoundEffectXAudio2.hpp"
#endif

#include "Pomdog/Audio/AudioBuffer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <utility>

namespace Pomdog {
namespace {

static std::shared_ptr<Details::SoundSystem::NativeAudioBuffer> GetNativeBuffer(std::shared_ptr<AudioBuffer> const& audioBuffer)
{
	std::shared_ptr<Details::SoundSystem::NativeAudioBuffer> nativeAudioBuffer(
		audioBuffer, audioBuffer->NativeAudioBuffer());
	return std::move(nativeAudioBuffer);
}

}// unnamed namespace
//-----------------------------------------------------------------------
SoundEffect::SoundEffect(std::shared_ptr<AudioBuffer> const& audioBuffer)
	: nativeSoundEffect(std::make_unique<Details::SoundSystem::NativeSoundEffect>(GetNativeBuffer(audioBuffer)))
	, pitch(0.0f)
	, volume(1.0f)
	, state(SoundState::Stopped)
	, isLooped(false)
{
}
//-----------------------------------------------------------------------
SoundEffect::~SoundEffect() = default;
//-----------------------------------------------------------------------
void SoundEffect::Pause()
{
	POMDOG_ASSERT(nativeSoundEffect);
	state = SoundState::Paused;
	nativeSoundEffect->Pause();
}
//-----------------------------------------------------------------------
void SoundEffect::Play()
{
	POMDOG_ASSERT(nativeSoundEffect);
	state = SoundState::Playing;
	nativeSoundEffect->Play();
}
//-----------------------------------------------------------------------
void SoundEffect::Stop()
{
	POMDOG_ASSERT(nativeSoundEffect);
	state = SoundState::Stopped;
	nativeSoundEffect->Stop();
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
void SoundEffect::IsLooped(bool isLoopedIn)
{
	POMDOG_ASSERT(nativeSoundEffect);
	this->isLooped = isLoopedIn;
	nativeSoundEffect->IsLooped(isLoopedIn);
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
//	//POMDOG_ASSERT(nativeSoundEffect);
//	//nativeSoundEffect->Pan(panpot);
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
}// namespace Pomdog
