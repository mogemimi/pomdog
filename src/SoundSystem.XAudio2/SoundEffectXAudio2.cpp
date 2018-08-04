// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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
namespace {

void BuildXAudioBuffer(
    const AudioClipXAudio2& audioClip,
    bool isLooped,
    XAUDIO2_BUFFER & bufferDesc)
{
    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = audioClip.SizeInBytes();
    bufferDesc.pAudioData = audioClip.Data();
    bufferDesc.PlayBegin = 0;
    bufferDesc.PlayLength = 0;
    bufferDesc.LoopBegin = 0;
    bufferDesc.LoopLength = 0;
    bufferDesc.LoopCount = 0;
    bufferDesc.pContext = nullptr;

    if (isLooped) {
        bufferDesc.LoopCount = XAUDIO2_LOOP_INFINITE;
    }
}

} // unnamed namespace

SoundEffectXAudio2::SoundEffectXAudio2(
    AudioEngineXAudio2 & audioEngine,
    const std::shared_ptr<AudioClipXAudio2>& audioClipIn,
    bool isLoopedIn)
    : audioClip(audioClipIn)
    , sourceVoice(nullptr)
    , isLooped(isLoopedIn)
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
    BuildXAudioBuffer(*audioClip, isLooped, bufferDesc);

    hr = sourceVoice->SubmitSourceBuffer(&bufferDesc);
    if (FAILED(hr)) {
        sourceVoice->DestroyVoice();
        sourceVoice = nullptr;

        // Error: FUS RO DAH!
        ///@todo Not implemented
    }
}

SoundEffectXAudio2::~SoundEffectXAudio2()
{
    if (sourceVoice) {
        this->Stop();
        sourceVoice->DestroyVoice();
        sourceVoice = nullptr;
    }

    audioClip.reset();
}

void SoundEffectXAudio2::ExitLoop()
{
    POMDOG_ASSERT(sourceVoice);
    sourceVoice->ExitLoop();
}

void SoundEffectXAudio2::Pause()
{
    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Stop();
}

void SoundEffectXAudio2::Play()
{
    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Start();
}

void SoundEffectXAudio2::Stop()
{
    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();

    XAUDIO2_BUFFER bufferDesc;
    BuildXAudioBuffer(*audioClip, isLooped, bufferDesc);

    sourceVoice->SubmitSourceBuffer(&bufferDesc);
}

void SoundEffectXAudio2::Apply3D(const AudioListener& listener, const AudioEmitter& emitter)
{
    ///@todo Not implemented
    UNREFERENCED_PARAMETER(listener);
    UNREFERENCED_PARAMETER(emitter);

    POMDOG_ASSERT(false);
}

void SoundEffectXAudio2::SetPitch(float pitch)
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

void SoundEffectXAudio2::SetVolume(float volume)
{
    POMDOG_ASSERT(sourceVoice);
    POMDOG_ASSERT(volume <= 1.0f);
    POMDOG_ASSERT(volume >= -1.0f);

    sourceVoice->SetVolume(volume);
}

} // namespace XAudio2
} // namespace SoundSystem
} // namespace Detail
} // namespace Pomdog
