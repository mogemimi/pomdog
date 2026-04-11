// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_source_xaudio2.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {
namespace {

void buildXAudio2BufferDesc(
    unsafe_ptr<AudioClipXAudio2> audioClip,
    bool isLooped,
    XAUDIO2_BUFFER& bufferDesc) noexcept
{
    POMDOG_ASSERT(audioClip != nullptr);

    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = static_cast<UINT32>(audioClip->getSizeInBytes());
    bufferDesc.pAudioData = reinterpret_cast<const BYTE*>(audioClip->getData());
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

AudioSourceXAudio2::AudioSourceXAudio2() noexcept = default;

AudioSourceXAudio2::~AudioSourceXAudio2() noexcept
{
    if (sourceVoice_ != nullptr) {
        stop();
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }

    audioClip_.reset();
}

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045)
std::unique_ptr<Error>
AudioSourceXAudio2::initialize(
    unsafe_ptr<IXAudio2> xAudio2,
    std::shared_ptr<AudioClipXAudio2> audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(xAudio2 != nullptr);
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(sourceVoice_ == nullptr);

    audioClip_ = std::move(audioClip);
    pitch_ = 0.0f;
    isLooped_ = isLooped;
    isPlaying_ = false;

    const auto bitsPerSample = audioClip_->getBitsPerSample();
    const auto sampleRate = audioClip_->getSampleRate();
    const auto channelCount = detail::toAudioChannelCount(audioClip_->getChannels());
    const auto blockAlign = channelCount * (bitsPerSample / 8);

    ::WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = static_cast<WORD>(channelCount);
    waveFormat.nSamplesPerSec = static_cast<DWORD>(sampleRate);
    waveFormat.nAvgBytesPerSec = static_cast<DWORD>(sampleRate * blockAlign);
    waveFormat.nBlockAlign = static_cast<WORD>(blockAlign);
    waveFormat.wBitsPerSample = static_cast<WORD>(bitsPerSample);
    waveFormat.cbSize = 0;

    if (auto hr = xAudio2->CreateSourceVoice(&sourceVoice_, &waveFormat); FAILED(hr)) {
        return errors::make("CreateSourceVoice() failed: " + std::to_string(hr));
    }

    XAUDIO2_BUFFER bufferDesc = {};
    buildXAudio2BufferDesc(audioClip_.get(), isLooped_, bufferDesc);

    if (auto hr = sourceVoice_->SubmitSourceBuffer(&bufferDesc); FAILED(hr)) {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
        return errors::make("SubmitSourceBuffer() failed: " + std::to_string(hr));
    }

    return nullptr;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

void AudioSourceXAudio2::pause() noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->Stop();
    isPlaying_ = false;
}

void AudioSourceXAudio2::play() noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->Start();
    isPlaying_ = true;

    XAUDIO2_VOICE_STATE state;
    sourceVoice_->GetState(&state, 0);
    if (state.BuffersQueued == 0) {
        XAUDIO2_BUFFER bufferDesc = {};
        buildXAudio2BufferDesc(audioClip_.get(), isLooped_, bufferDesc);

        sourceVoice_->SubmitSourceBuffer(&bufferDesc);
    }
}

void AudioSourceXAudio2::stop() noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->Stop();
    isPlaying_ = false;

    sourceVoice_->FlushSourceBuffers();
}

bool AudioSourceXAudio2::isLooped() const noexcept
{
    return isLooped_;
}

void AudioSourceXAudio2::exitLoop() noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);

    if (!isLooped_) {
        return;
    }

    sourceVoice_->ExitLoop();
    isLooped_ = false;
}

bool AudioSourceXAudio2::isPlaying() const noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);

    XAUDIO2_VOICE_STATE state;
    sourceVoice_->GetState(&state, 0);
    if (isPlaying_ && (state.BuffersQueued > 0)) {
        return true;
    }
    return false;
}

f32 AudioSourceXAudio2::getPitch() const noexcept
{
    return pitch_;
}

void AudioSourceXAudio2::setPitch(f32 pitch) noexcept
{
    POMDOG_ASSERT(!std::isnan(pitch));
    POMDOG_ASSERT(pitch >= -1.0f);
    POMDOG_ASSERT(pitch <= 1.0f);

    pitch_ = pitch;

    // NOTE: Convert from pitch to nativePitch:
    //       +1.0f => 2.0f (up one octave)
    //        0.0f => 1.0f (normal)
    //       -1.0f => 0.5f (down one octave)
    const auto nativePitch = std::pow(2.0f, pitch_);

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(nativePitch >= 0.0f);
    sourceVoice_->SetFrequencyRatio(nativePitch);
}

f32 AudioSourceXAudio2::getVolume() const noexcept
{
    f32 volume = 0.0f;
    sourceVoice_->GetVolume(&volume);
    return volume;
}

void AudioSourceXAudio2::setVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(volume >= 0.0f);
    POMDOG_ASSERT(volume <= 1.0f);
    sourceVoice_->SetVolume(volume);
}

} // namespace pomdog::detail::xaudio2
