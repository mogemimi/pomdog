// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "Pomdog/Audio/SoundEffect.hpp"
#include "Pomdog/Audio/SoundState.hpp"
#include <memory>
#include <optional>

namespace Pomdog {
class AudioEmitter;
class AudioListener;
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::OpenAL {

class AudioClipAL;
class AudioEngineAL;

class SoundEffectAL final : public SoundEffect {
private:
    std::shared_ptr<AudioClipAL> audioClip;
    std::optional<ALuint> source;

    float pitch = 0.0f;
    float volume = 1.0f;
    SoundState state = SoundState::Stopped;
    bool isLooped = false;

public:
    SoundEffectAL() noexcept;

    SoundEffectAL(const SoundEffectAL&) = delete;
    SoundEffectAL& operator=(const SoundEffectAL&) = delete;

    ~SoundEffectAL() noexcept override;

    /// Initializes the audio engine.
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        const std::shared_ptr<AudioClipAL>& audioClip,
        bool isLooped) noexcept;

    /// Pauses the sound.
    void
    Pause() noexcept override;

    /// Plays or resumes playing the sound.
    void
    Play() noexcept override;

    /// Stops playing the sound immediately.
    void
    Stop() noexcept override;

    /// Applies 3D positioning to the sound.
    void
    Apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept override;

    /// Returns true if the audio clip is looping, false otherwise.
    [[nodiscard]] bool
    IsLooped() const noexcept override;

    /// Stops looping the sound when it reaches the end of the sound.
    void
    ExitLoop() noexcept override;

    /// Gets the current state of the audio source.
    [[nodiscard]] SoundState
    GetState() const noexcept override;

    /// Gets the pitch of the audio source.
    [[nodiscard]] float
    GetPitch() const noexcept override;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    void
    SetPitch(float pitch) noexcept override;

    /// Gets the volume of the audio source.
    [[nodiscard]] float
    GetVolume() const noexcept override;

    /// Sets the volume of the audio source (0.0 to 1.0).
    void
    SetVolume(float volume) noexcept override;
};

} // namespace Pomdog::Detail::OpenAL
