// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/SoundState.hpp"
#include "Pomdog/Audio/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SoundEffect final {
public:
    SoundEffect(
        AudioEngine& audioEngine,
        const std::shared_ptr<AudioClip>& audioClip,
        bool isLooped);

    ~SoundEffect();

    SoundEffect(const SoundEffect&) = delete;
    SoundEffect(SoundEffect&&) = default;
    SoundEffect& operator=(const SoundEffect&) = delete;
    SoundEffect& operator=(SoundEffect&&) = default;

    /// Pauses the sound.
    void Pause();

    /// Plays or resumes playing the sound.
    void Play();

    /// Stops playing the sound immediately.
    void Stop();

    /// Applies 3D positioning to the sound.
    void Apply3D(const AudioListener& listener, const AudioEmitter& emitter);

    /// Returns true if the audio clip is looping, false otherwise.
    bool IsLooped() const;

    /// Stops looping the sound when it reaches the end of the sound.
    void ExitLoop();

    /// Gets the current state of the audio source.
    SoundState GetState() const;

    /// Gets the panning of the audio source.
    float GetPan() const;

    /// Sets the panning of the audio source (-1.0 to 1.0).
    void SetPan(float panpot);

    /// Gets the pitch of the audio source.
    float GetPitch() const;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    void SetPitch(float pitch);

    /// Gets the volume of the audio source.
    float GetVolume() const;

    /// Sets the volume of the audio source (0.0 to 1.0).
    void SetVolume(float volume);

private:
    std::unique_ptr<Detail::SoundSystem::NativeSoundEffect> nativeSoundEffect;
    float pitch;
    float volume;
    SoundState state;
    bool isLooped;
};

} // namespace Pomdog
