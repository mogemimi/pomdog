// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/sound_effect_xaudio2.h"
#include "pomdog/audio/audio_emitter.h"
#include "pomdog/audio/audio_listener.h"
#include "pomdog/audio/xaudio2/audio_clip_xaudio2.h"
#include "pomdog/audio/xaudio2/audio_engine_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {
namespace {

void buildXAudio2Buffer(
    const AudioClipXAudio2& audioClip,
    bool isLooped,
    XAUDIO2_BUFFER& bufferDesc) noexcept
{
    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = static_cast<UINT32>(audioClip.getSizeInBytes());
    bufferDesc.pAudioData = audioClip.getData();
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
    if (sourceVoice_ != nullptr) {
        stop();
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
    }

    audioClip_.reset();
}

std::unique_ptr<Error>
SoundEffectXAudio2::initialize(
    IXAudio2* xAudio2,
    const std::shared_ptr<AudioClipXAudio2>& audioClipIn,
    bool isLoopedIn) noexcept
{
    audioClip_ = audioClipIn;
    isLooped_ = isLoopedIn;

    POMDOG_ASSERT(xAudio2 != nullptr);
    POMDOG_ASSERT(audioClip_ != nullptr);
    POMDOG_ASSERT(audioClip_->getWaveFormat() != nullptr);

    if (auto hr = xAudio2->CreateSourceVoice(&sourceVoice_, audioClip_->getWaveFormat()); FAILED(hr)) {
        return errors::make("CreateSourceVoice() failed: " + std::to_string(hr));
    }

    XAUDIO2_BUFFER bufferDesc;
    buildXAudio2Buffer(*audioClip_, isLooped_, bufferDesc);

    if (auto hr = sourceVoice_->SubmitSourceBuffer(&bufferDesc); FAILED(hr)) {
        sourceVoice_->DestroyVoice();
        sourceVoice_ = nullptr;
        return errors::make("SubmitSourceBuffer() failed: " + std::to_string(hr));
    }

    return nullptr;
}

void SoundEffectXAudio2::pause() noexcept
{
    if (state_ == SoundState::Paused) {
        return;
    }

    POMDOG_ASSERT(sourceVoice_);
    sourceVoice_->Stop();
    state_ = SoundState::Paused;
}

void SoundEffectXAudio2::play() noexcept
{
    if (state_ == SoundState::Playing) {
        return;
    }

    POMDOG_ASSERT(sourceVoice_);
    sourceVoice_->Start();
    state_ = SoundState::Playing;
}

void SoundEffectXAudio2::stop() noexcept
{
    if (state_ == SoundState::Stopped) {
        return;
    }

    POMDOG_ASSERT(sourceVoice_);
    sourceVoice_->Stop();
    sourceVoice_->FlushSourceBuffers();

    XAUDIO2_BUFFER bufferDesc;
    buildXAudio2Buffer(*audioClip_, isLooped_, bufferDesc);

    sourceVoice_->SubmitSourceBuffer(&bufferDesc);
    state_ = SoundState::Stopped;
}

void SoundEffectXAudio2::apply3D(
    [[maybe_unused]] const AudioListener& listener,
    [[maybe_unused]] const AudioEmitter& emitter) noexcept
{
    POMDOG_ASSERT(false);
}

bool SoundEffectXAudio2::isLooped() const noexcept
{
    return isLooped_;
}

void SoundEffectXAudio2::exitLoop() noexcept
{
    if (!isLooped_) {
        return;
    }

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->ExitLoop();
    isLooped_ = false;
}

SoundState
SoundEffectXAudio2::getState() const noexcept
{
    return state_;
}

float SoundEffectXAudio2::getPitch() const noexcept
{
    return pitch_;
}

void SoundEffectXAudio2::setPitch(float pitchIn) noexcept
{
    POMDOG_ASSERT(!std::isnan(pitchIn));
    POMDOG_ASSERT(pitchIn >= -1.0f);
    POMDOG_ASSERT(pitchIn <= 1.0f);

    pitch_ = pitchIn;

    // NOTE: Convert from pitch to nativePitch:
    // +1.0f => 2.0f (up one octave)
    //  0.0f => 1.0f (normal)
    // -1.0f => 0.5f (down one octave)

    const auto nativePitch = std::pow(2.0f, pitch_);

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(nativePitch >= 0.0f);
    sourceVoice_->SetFrequencyRatio(nativePitch);
}

float SoundEffectXAudio2::getVolume() const noexcept
{
    return volume_;
}

void SoundEffectXAudio2::setVolume(float volumeIn) noexcept
{
    POMDOG_ASSERT(volumeIn >= 0.0f);
    POMDOG_ASSERT(volumeIn <= 1.0f);

    volume_ = volumeIn;

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(volume_ >= 0.0f);
    sourceVoice_->SetVolume(volume_);
}

} // namespace pomdog::detail::xaudio2
