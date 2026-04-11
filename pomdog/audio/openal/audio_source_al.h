// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_source.h"
#include "pomdog/audio/openal/openal_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {
class AudioClipAL;
} // namespace pomdog::detail::openal

namespace pomdog::detail::openal {

/// Non-streaming audio source implementation for OpenAL.
class AudioSourceAL final : public AudioSource {
private:
    std::shared_ptr<AudioClipAL> audioClip_;
    std::optional<ALuint> source_;
    f32 pitch_ = 0.0f;
    f32 volume_ = 0.0f;
    bool isLooped_ = false;

public:
    AudioSourceAL() noexcept;

    AudioSourceAL(const AudioSourceAL&) = delete;
    AudioSourceAL& operator=(const AudioSourceAL&) = delete;

    ~AudioSourceAL() noexcept override;

    /// Initializes the audio source.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::shared_ptr<AudioClipAL> audioClip,
        bool isLooped) noexcept;

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
