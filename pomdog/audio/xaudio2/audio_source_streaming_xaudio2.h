// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_source.h"
#include "pomdog/audio/xaudio2/xaudio2_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <atomic>
#include <memory>
#include <mutex>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::xaudio2 {
class AudioClipStreamingXAudio2;
} // namespace pomdog::detail::xaudio2

namespace pomdog::detail::xaudio2 {

/// Streaming audio source implementation for XAudio2.
///
/// Uses a triple-buffer system with thread-safe polling to stream
/// audio data progressively during playback.
class AudioSourceStreamingXAudio2 final : public AudioSource {
private:
    std::shared_ptr<AudioClipStreamingXAudio2> audioClip_ = {};
    std::array<std::vector<u8>, 3> buffers_;
    std::mutex sourceMutex_ = {};
    std::mutex clipMutex_ = {};
    unsafe_ptr<IXAudio2SourceVoice> sourceVoice_ = nullptr;
    f32 pitch_ = 0.0f;
    u8 nextBufferIndex_ = 0;
    std::atomic<bool> isPlaying_ = false;
    std::atomic<bool> isLooped_ = false;
    std::atomic<bool> canQueueBuffers_ = false;

public:
    AudioSourceStreamingXAudio2() noexcept;

    AudioSourceStreamingXAudio2(const AudioSourceStreamingXAudio2&) = delete;
    AudioSourceStreamingXAudio2& operator=(const AudioSourceStreamingXAudio2&) = delete;

    ~AudioSourceStreamingXAudio2() noexcept override;

    /// Initializes the streaming audio source.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        unsafe_ptr<IXAudio2> xAudio2,
        std::shared_ptr<AudioClipStreamingXAudio2> audioClip,
        bool isLooped) noexcept;

    /// Polls for completed buffers and queues new audio data.
    /// Called from a background thread by AudioEngineXAudio2.
    [[nodiscard]] std::unique_ptr<Error>
    poll() noexcept;

    /// Pauses the sound.
    void
    pause() noexcept override;

    /// Plays or resumes playing the sound.
    void
    play() noexcept override;

    /// Stops playing the sound immediately.
    void
    stop() noexcept override;

    /// Returns true if the audio source is set to loop, false otherwise.
    [[nodiscard]] bool
    isLooped() const noexcept override;

    /// Stops looping the sound when it reaches the end of the sound.
    void
    exitLoop() noexcept override;

    /// Returns true if the audio source is currently playing, false otherwise.
    [[nodiscard]] bool
    isPlaying() const noexcept override;

    /// Gets the pitch of the audio source.
    [[nodiscard]] f32
    getPitch() const noexcept override;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    void
    setPitch(f32 pitch) noexcept override;

    /// Gets the volume of the audio source.
    [[nodiscard]] f32
    getVolume() const noexcept override;

    /// Sets the volume of the audio source (0.0 to 1.0).
    void
    setVolume(f32 volume) noexcept override;
};

} // namespace pomdog::detail::xaudio2
