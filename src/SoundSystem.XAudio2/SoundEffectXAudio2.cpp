//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SoundEffectXAudio2.hpp"
#include "AudioClipXAudio2.hpp"
#include "AudioEngineXAudio2.hpp"
#include "Pomdog/Audio/AudioEmitter.hpp"
#include "Pomdog/Audio/AudioListener.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <cmath>

namespace Pomdog {
namespace Detail {
namespace SoundSystem {
namespace XAudio2 {
//-----------------------------------------------------------------------
SoundEffectXAudio2::SoundEffectXAudio2(AudioEngineXAudio2 & audioEngine,
	std::shared_ptr<AudioClipXAudio2> const& audioClipIn, bool isLooped)
	: audioClip(audioClipIn)
	, sourceVoice(nullptr)
{
	auto xAudio2 = audioEngine.XAudio2Engine();

	POMDOG_ASSERT(xAudio2);
	POMDOG_ASSERT(audioClip);
	POMDOG_ASSERT(audioClip->WaveFormat() != nullptr);

	HRESULT hr = xAudio2->CreateSourceVoice(&sourceVoice, audioClip->WaveFormat());
	if (FAILED(hr)) {
		// Error: FUS RO DAH!
		///@todo Not implemented
	}

	XAUDIO2_BUFFER bufferDesc;
	bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
	bufferDesc.AudioBytes = audioClip->SizeInBytes();
	bufferDesc.pAudioData = audioClip->Data();
	bufferDesc.PlayBegin = 0;
	bufferDesc.PlayLength = 0;
	bufferDesc.LoopBegin = 0;
	bufferDesc.LoopLength = 0;
	bufferDesc.LoopCount = 0;
	bufferDesc.pContext = nullptr;

	if (isLooped) {
		bufferDesc.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	hr = sourceVoice->SubmitSourceBuffer(&bufferDesc);
	if (FAILED(hr)) {
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;

		// Error: FUS RO DAH!
		///@todo Not implemented
	}
}
//-----------------------------------------------------------------------
SoundEffectXAudio2::~SoundEffectXAudio2()
{
	if (sourceVoice)
	{
		this->Stop();
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}

	audioClip.reset();
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::ExitLoop()
{
	POMDOG_ASSERT(sourceVoice);
	sourceVoice->ExitLoop();
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Pause()
{
	POMDOG_ASSERT(sourceVoice);
	sourceVoice->Stop();
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Play()
{
	POMDOG_ASSERT(sourceVoice);
	sourceVoice->Start();
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Stop()
{
	POMDOG_ASSERT(sourceVoice);
	sourceVoice->Stop();
	sourceVoice->FlushSourceBuffers();
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Apply3D(AudioListener const& listener, AudioEmitter const& emitter)
{
	///@todo Not implemented
	UNREFERENCED_PARAMETER(listener);
	UNREFERENCED_PARAMETER(emitter);

	POMDOG_ASSERT(false);
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Pitch(float pitch)
{
	POMDOG_ASSERT(sourceVoice);
	POMDOG_ASSERT(!std::isnan(pitch));
	POMDOG_ASSERT(pitch <= 1.0f);
	POMDOG_ASSERT(pitch >= -1.0f);

	// pitch --> nativePitch
	// +1.0f --> 2.0f (up one octave)
	//  0.0f --> 1.0f (normal)
	// -1.0f --> 0.5f (down one octave)

	auto nativePitch = std::pow(2.0f, pitch);

	sourceVoice->SetFrequencyRatio(nativePitch);
}
//-----------------------------------------------------------------------
void SoundEffectXAudio2::Volume(float volume)
{
	POMDOG_ASSERT(sourceVoice);
	POMDOG_ASSERT(volume <= 1.0f);
	POMDOG_ASSERT(volume >= -1.0f);

	sourceVoice->SetVolume(volume);
}
//-----------------------------------------------------------------------
}// namespace XAudio2
}// namespace SoundSystem
}// namespace Detail
}// namespace Pomdog
