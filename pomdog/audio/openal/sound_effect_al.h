// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/openal/prerequisites_openal.h"
#include "pomdog/audio/sound_effect.h"
#include "pomdog/audio/sound_state.h"
#include <memory>
#include <optional>

namespace pomdog {
class AudioEmitter;
class AudioListener;
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {

class AudioClipAL;
class AudioEngineAL;

class SoundEffectAL final : public SoundEffect {
private:
    std::shared_ptr<AudioClipAL> audioClip_;
    std::optional<ALuint> source_;

    float pitch_ = 0.0f;
    float volume_ = 1.0f;
    SoundState state_ = SoundState::Stopped;
    bool isLooped_ = false;

public:
    SoundEffectAL() noexcept;

    SoundEffectAL(const SoundEffectAL&) = delete;
    SoundEffectAL& operator=(const SoundEffectAL&) = delete;

    ~SoundEffectAL() noexcept override;

    /// Initializes the audio engine.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<AudioClipAL>& audioClip,
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

    /// Applies 3D positioning to the sound.
    void
    apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept override;

    /// Returns true if the audio clip is looping, false otherwise.
    [[nodiscard]] bool
    isLooped() const noexcept override;

    /// Stops looping the sound when it reaches the end of the sound.
    void
    exitLoop() noexcept override;

    /// Gets the current state of the audio source.
    [[nodiscard]] SoundState
    getState() const noexcept override;

    /// Gets the pitch of the audio source.
    [[nodiscard]] float
    getPitch() const noexcept override;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    void
    setPitch(float pitch) noexcept override;

    /// Gets the volume of the audio source.
    [[nodiscard]] float
    getVolume() const noexcept override;

    /// Sets the volume of the audio source (0.0 to 1.0).
    void
    setVolume(float volume) noexcept override;
};

} // namespace pomdog::detail::openal
