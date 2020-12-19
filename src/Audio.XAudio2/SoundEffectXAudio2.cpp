// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "SoundEffectXAudio2.hpp"
#include "AudioClipXAudio2.hpp"
#include "AudioEngineXAudio2.hpp"
#include "Pomdog/Audio/AudioEmitter.hpp"
#include "Pomdog/Audio/AudioListener.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <cmath>

namespace Pomdog::Detail::XAudio2 {
namespace {

void BuildXAudio2Buffer(
    const AudioClipXAudio2& audioClip,
    bool isLooped,
    XAUDIO2_BUFFER& bufferDesc) noexcept
{
    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = static_cast<UINT32>(audioClip.GetSizeInBytes());
    bufferDesc.pAudioData = audioClip.GetData();
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

} // namespace

SoundEffectXAudio2::SoundEffectXAudio2() noexcept = default;

SoundEffectXAudio2::~SoundEffectXAudio2()
{
    if (sourceVoice != nullptr) {
        this->Stop();
        sourceVoice->DestroyVoice();
        sourceVoice = nullptr;
    }

    audioClip.reset();
}

std::unique_ptr<Error>
SoundEffectXAudio2::Initialize(
    IXAudio2* xAudio2,
    const std::shared_ptr<AudioClipXAudio2>& audioClipIn,
    bool isLoopedIn) noexcept
{
    audioClip = audioClipIn;
    isLooped = isLoopedIn;

    POMDOG_ASSERT(xAudio2 != nullptr);
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(audioClip->GetWaveFormat() != nullptr);

    if (auto hr = xAudio2->CreateSourceVoice(&sourceVoice, audioClip->GetWaveFormat()); FAILED(hr)) {
        return Errors::New("CreateSourceVoice() failed: " + std::to_string(hr));
    }

    XAUDIO2_BUFFER bufferDesc;
    BuildXAudio2Buffer(*audioClip, isLooped, bufferDesc);

    if (auto hr = sourceVoice->SubmitSourceBuffer(&bufferDesc); FAILED(hr)) {
        sourceVoice->DestroyVoice();
        sourceVoice = nullptr;
        return Errors::New("SubmitSourceBuffer() failed: " + std::to_string(hr));
    }

    return nullptr;
}

void SoundEffectXAudio2::Pause() noexcept
{
    if (this->state == SoundState::Paused) {
        return;
    }

    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Stop();
    this->state = SoundState::Paused;
}

void SoundEffectXAudio2::Play() noexcept
{
    if (this->state == SoundState::Playing) {
        return;
    }

    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Start();
    this->state = SoundState::Playing;
}

void SoundEffectXAudio2::Stop() noexcept
{
    if (this->state == SoundState::Stopped) {
        return;
    }

    POMDOG_ASSERT(sourceVoice);
    sourceVoice->Stop();
    sourceVoice->FlushSourceBuffers();

    XAUDIO2_BUFFER bufferDesc;
    BuildXAudio2Buffer(*audioClip, isLooped, bufferDesc);

    sourceVoice->SubmitSourceBuffer(&bufferDesc);
    this->state = SoundState::Stopped;
}

void SoundEffectXAudio2::Apply3D(
    [[maybe_unused]] const AudioListener& listener,
    [[maybe_unused]] const AudioEmitter& emitter) noexcept
{
    POMDOG_ASSERT(false);
}

bool SoundEffectXAudio2::IsLooped() const noexcept
{
    return this->isLooped;
}

void SoundEffectXAudio2::ExitLoop() noexcept
{
    if (!this->isLooped) {
        return;
    }

    POMDOG_ASSERT(sourceVoice != nullptr);
    sourceVoice->ExitLoop();
    this->isLooped = false;
}

SoundState SoundEffectXAudio2::GetState() const noexcept
{
    return state;
}

float SoundEffectXAudio2::GetPitch() const noexcept
{
    return pitch;
}

void SoundEffectXAudio2::SetPitch(float pitchIn) noexcept
{
    POMDOG_ASSERT(!std::isnan(pitchIn));
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);

    this->pitch = pitchIn;

    // NOTE: Convert from pitch to nativePitch:
    // +1.0f => 2.0f (up one octave)
    //  0.0f => 1.0f (normal)
    // -1.0f => 0.5f (down one octave)

    auto nativePitch = std::pow(2.0f, pitch);

    POMDOG_ASSERT(sourceVoice != nullptr);
    POMDOG_ASSERT(nativePitch >= 0.0f);
    sourceVoice->SetFrequencyRatio(nativePitch);
}

float SoundEffectXAudio2::GetVolume() const noexcept
{
    return volume;
}

void SoundEffectXAudio2::SetVolume(float volumeIn) noexcept
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);

    this->volume = volumeIn;

    POMDOG_ASSERT(sourceVoice != nullptr);
    POMDOG_ASSERT(volume >= 0.0f);
    sourceVoice->SetVolume(volume);
}

} // namespace Pomdog::Detail::XAudio2
