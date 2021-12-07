// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/sound_effect.hpp"
#include "pomdog/audio/sound_state.hpp"
#include "pomdog/audio/xaudio2/prerequisites_xaudio2.hpp"
#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {
class AudioEmitter;
class AudioListener;
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::XAudio2 {

class AudioClipXAudio2;

class SoundEffectXAudio2 final : public SoundEffect {
private:
    std::shared_ptr<AudioClipXAudio2> audioClip;
    IXAudio2SourceVoice* sourceVoice = nullptr;

    float pitch = 0.0f;
    float volume = 1.0f;
    SoundState state = SoundState::Stopped;
    bool isLooped = false;

public:
    SoundEffectXAudio2() noexcept;

    SoundEffectXAudio2(const SoundEffectXAudio2&) = delete;
    SoundEffectXAudio2& operator=(const SoundEffectXAudio2&) = delete;

    ~SoundEffectXAudio2() noexcept;

    /// Initializes the audio engine.
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        IXAudio2* xAudio2,
        const std::shared_ptr<AudioClipXAudio2>& audioClip,
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

} // namespace Pomdog::Detail::XAudio2
