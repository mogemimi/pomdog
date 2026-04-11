// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/audio/xaudio2/audio_source_streaming_xaudio2.h"
#include "pomdog/audio/details/audio_clip_helpers.h"
#include "pomdog/audio/xaudio2/audio_clip_streaming_xaudio2.h"
#include "pomdog/memory/memset_span.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::xaudio2 {

AudioSourceStreamingXAudio2::AudioSourceStreamingXAudio2() noexcept = default;

AudioSourceStreamingXAudio2::~AudioSourceStreamingXAudio2() noexcept
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
AudioSourceStreamingXAudio2::initialize(
    unsafe_ptr<IXAudio2> xAudio2,
    std::shared_ptr<AudioClipStreamingXAudio2> audioClip,
    bool isLooped) noexcept
{
    POMDOG_ASSERT(xAudio2 != nullptr);
    POMDOG_ASSERT(audioClip != nullptr);
    POMDOG_ASSERT(sourceVoice_ == nullptr);

    audioClip_ = std::move(audioClip);
    pitch_ = 0.0f;
    isLooped_ = isLooped;
    isPlaying_ = false;
    canQueueBuffers_ = true;

    const auto samplesPerSec = audioClip_->getSampleRate();
    const auto bitsPerSample = audioClip_->getBitsPerSample();
    const auto channelCount = detail::toAudioChannelCount(audioClip_->getChannels());
    const auto blockAlign = channelCount * (bitsPerSample / 8);

    POMDOG_ASSERT(samplesPerSec > 0);
    POMDOG_ASSERT(bitsPerSample >= 8);

    ::WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = static_cast<WORD>(channelCount);
    waveFormat.nSamplesPerSec = static_cast<DWORD>(samplesPerSec);
    waveFormat.nAvgBytesPerSec = static_cast<DWORD>(samplesPerSec * blockAlign);
    waveFormat.nBlockAlign = static_cast<WORD>(blockAlign);
    waveFormat.wBitsPerSample = static_cast<WORD>(bitsPerSample);
    waveFormat.cbSize = 0;

    if (auto hr = xAudio2->CreateSourceVoice(&sourceVoice_, &waveFormat); FAILED(hr)) {
        return errors::make("CreateSourceVoice() failed: " + std::to_string(hr));
    }

    if (auto err = audioClip_->seekStart(); err != nullptr) {
        return errors::wrap(std::move(err), "seekStart() failed");
    }

    for (auto& buf : buffers_) {
        // NOTE: 4KiB audio temporary buffer for streaming
        constexpr u32 tempBufferSize = 1024 * 4;

        buf.clear();
        buf.resize(tempBufferSize);
    }
    nextBufferIndex_ = 0;

    return nullptr;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(5045)
std::unique_ptr<Error>
AudioSourceStreamingXAudio2::poll() noexcept
{
    if (!isPlaying_) {
        return nullptr;
    }

    const u32 maxBufferCount = static_cast<u32>(buffers_.size());

    ::XAUDIO2_VOICE_STATE state = {};
    sourceVoice_->GetState(&state);

    if (state.BuffersQueued >= maxBufferCount) {
        return nullptr;
    }

    if (!canQueueBuffers_) {
        if (state.BuffersQueued == 0) {
            isPlaying_ = false;
        }
        return nullptr;
    }

    const auto bitsPerSample = audioClip_->getBitsPerSample();
    const auto channels = audioClip_->getChannels();

    const auto bufferIndex = nextBufferIndex_;
    nextBufferIndex_ += 1;
    if (nextBufferIndex_ >= maxBufferCount) {
        nextBufferIndex_ = 0;
    }

    POMDOG_ASSERT(bufferIndex < buffers_.size());
    std::span<u8> bufferView{buffers_[bufferIndex]};

    {
        const bool looped = isLooped_.load();

        std::lock_guard<std::mutex> lock(clipMutex_);

        auto [sampleCount, sampleErr] = audioClip_->readSamplesInterleaved(bufferView, looped);
        if (sampleErr != nullptr) {
            return errors::wrap(std::move(sampleErr), "readSamplesInterleaved() failed");
        }

        if (sampleCount <= 0) {
            if (auto err = audioClip_->seekStart(); err != nullptr) {
                return errors::wrap(std::move(err), "seekStart() failed");
            }
            if (!looped) {
                canQueueBuffers_ = false;
            }
            return nullptr;
        }
        bufferView = bufferView.first(static_cast<u32>((bitsPerSample / 8) * detail::toAudioChannelCount(channels) * sampleCount));
    }

    XAUDIO2_BUFFER bufferDesc = {};
    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = static_cast<UINT32>(bufferView.size());
    bufferDesc.pAudioData = bufferView.data();
    bufferDesc.PlayBegin = 0;
    bufferDesc.PlayLength = 0;
    bufferDesc.LoopBegin = 0;
    bufferDesc.LoopLength = 0;
    bufferDesc.LoopCount = 0;
    bufferDesc.pContext = nullptr;

    {
        std::lock_guard<std::mutex> lock(sourceMutex_);

        if (auto hr = sourceVoice_->SubmitSourceBuffer(&bufferDesc); FAILED(hr)) {
            return errors::make("SubmitSourceBuffer() failed: " + std::to_string(hr));
        }
    }

    return nullptr;
}
POMDOG_MSVC_SUPPRESS_WARNING_POP

void AudioSourceStreamingXAudio2::pause() noexcept
{
    isPlaying_ = false;

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->Stop();
}

void AudioSourceStreamingXAudio2::play() noexcept
{
    isPlaying_ = true;
    canQueueBuffers_ = true;

    POMDOG_ASSERT(sourceVoice_ != nullptr);
    sourceVoice_->Start();
}

void AudioSourceStreamingXAudio2::stop() noexcept
{
    isPlaying_ = false;

    {
        std::lock_guard<std::mutex> lock(sourceMutex_);

        POMDOG_ASSERT(sourceVoice_ != nullptr);
        sourceVoice_->Stop();
        sourceVoice_->FlushSourceBuffers();
    }
    {
        std::lock_guard<std::mutex> lock(clipMutex_);
        if (auto err = audioClip_->seekStart(); err != nullptr) {
            // TODO: error handling
        }
    }
    canQueueBuffers_ = true;
}

bool AudioSourceStreamingXAudio2::isLooped() const noexcept
{
    return isLooped_;
}

void AudioSourceStreamingXAudio2::exitLoop() noexcept
{
    if (!isLooped_) {
        return;
    }

    isLooped_ = false;
}

bool AudioSourceStreamingXAudio2::isPlaying() const noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);

    XAUDIO2_VOICE_STATE state;
    sourceVoice_->GetState(&state, 0);
    if (isPlaying_ && (state.BuffersQueued > 0)) {
        return true;
    }
    return false;
}

f32 AudioSourceStreamingXAudio2::getPitch() const noexcept
{
    return pitch_;
}

void AudioSourceStreamingXAudio2::setPitch(f32 pitch) noexcept
{
    POMDOG_ASSERT(!std::isnan(pitch));
    POMDOG_ASSERT(pitch >= -1.0f);
    POMDOG_ASSERT(pitch <= 1.0f);

    pitch_ = pitch;

    const auto nativePitch = std::pow(2.0f, pitch_);
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(nativePitch >= 0.0f);
    sourceVoice_->SetFrequencyRatio(nativePitch);
}

f32 AudioSourceStreamingXAudio2::getVolume() const noexcept
{
    f32 volume = 0.0f;
    sourceVoice_->GetVolume(&volume);
    return volume;
}

void AudioSourceStreamingXAudio2::setVolume(f32 volume) noexcept
{
    POMDOG_ASSERT(sourceVoice_ != nullptr);
    POMDOG_ASSERT(volume >= 0.0f);
    POMDOG_ASSERT(volume <= 1.0f);
    sourceVoice_->SetVolume(volume);
}

} // namespace pomdog::detail::xaudio2
