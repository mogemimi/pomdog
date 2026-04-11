// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_source.h"
#include "pomdog/audio/openal/openal_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/fixed_vector.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {
class AudioClipStreamingAL;
} // namespace pomdog::detail::openal

namespace pomdog::detail::openal {

/// Streaming audio source implementation for OpenAL.
///
/// Uses a 4-buffer queue to stream audio data progressively
/// from an AudioClipStreamingAL during playback.
class AudioSourceStreamingAL final : public AudioSource {
private:
    enum class QueueScheduleKind : u8 {
        CanEnqueue,
        DequeueOnly,
        ExitLoop,
        Stop,
    };

    std::shared_ptr<AudioClipStreamingAL> audioClip_;
    std::array<ALuint, 4> buffers_;
    FixedVector<ALuint, 4> waitBuffers_;
    std::optional<ALuint> source_;
    f32 pitch_ = 0.0f;
    f32 volume_ = 0.0f;
    QueueScheduleKind queueSchedule_ = QueueScheduleKind::CanEnqueue;
    bool isLooped_ = false;

public:
    AudioSourceStreamingAL() noexcept;

    AudioSourceStreamingAL(const AudioSourceStreamingAL&) = delete;
    AudioSourceStreamingAL& operator=(const AudioSourceStreamingAL&) = delete;

    ~AudioSourceStreamingAL() noexcept override;

    /// Initializes the streaming audio source.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<AudioClipStreamingAL> audioClip,
        std::span<u8> tempBuffer,
        bool isLooped) noexcept;

    /// Polls for processed buffers and queues new audio data.
    /// Called by AudioEngineAL::update() each frame.
    [[nodiscard]] std::unique_ptr<Error>
    poll(std::span<u8> tempBuffer) noexcept;

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

} // namespace pomdog::detail::openal
