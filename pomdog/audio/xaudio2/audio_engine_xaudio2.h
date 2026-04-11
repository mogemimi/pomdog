// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_engine.h"
#include "pomdog/audio/xaudio2/xaudio2_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <atomic>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::xaudio2 {
class AudioSourceStreamingXAudio2;
} // namespace pomdog::detail::xaudio2

namespace pomdog::detail::xaudio2 {

class AudioEngineXAudio2 final : public AudioEngine {
private:
    Microsoft::WRL::ComPtr<IXAudio2> xAudio2_ = {};
    std::mutex mutex_ = {};
    std::thread thread_ = {};
    std::vector<std::weak_ptr<AudioSourceStreamingXAudio2>> streamSources_ = {};
    unsafe_ptr<IXAudio2MasteringVoice> mainVoice_ = nullptr;
    std::atomic<bool> exitLoop_ = false;
    bool comInitialized_ = false;

public:
    AudioEngineXAudio2() noexcept;

    AudioEngineXAudio2(const AudioEngineXAudio2&) = delete;
    AudioEngineXAudio2& operator=(const AudioEngineXAudio2&) = delete;

    ~AudioEngineXAudio2() noexcept override;

    /// Initializes the audio engine.
    [[nodiscard]] std::unique_ptr<Error>
    initialize() noexcept;

    /// Polls streaming audio sources from a background thread.
    void
    update() noexcept;

    /// Creates a non-streaming audio clip from raw PCM data.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::span<const u8> waveData,
        i32 sampleRate,
        i32 bitsPerSample,
        AudioChannels channels) noexcept override;

    /// Creates a streaming audio clip from an audio clip file.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioClip>, std::unique_ptr<Error>>
    createAudioClip(
        std::unique_ptr<detail::AudioClipFile> audioClipFile) noexcept override;

    /// Creates an audio source for playing back the specified audio clip.
    [[nodiscard]] std::tuple<std::shared_ptr<AudioSource>, std::unique_ptr<Error>>
    createAudioSource(
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped) noexcept override;

    /// Gets the main volume that affects all audio output.
    [[nodiscard]] f32
    getMainVolume() const noexcept override;

    /// Sets the main volume that affects all audio output.
    void
    setMainVolume(f32 volume) noexcept override;

    /// Gets the pointer of the IXAudio2 object.
    [[nodiscard]] unsafe_ptr<IXAudio2>
    getXAudio2Engine() const noexcept;
};

} // namespace pomdog::detail::xaudio2
