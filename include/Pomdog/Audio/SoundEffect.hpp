// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Audio/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <memory>

namespace Pomdog {

class POMDOG_EXPORT SoundEffect {
public:
    virtual ~SoundEffect() noexcept;

    /// Pauses the sound.
    virtual void
    Pause() noexcept = 0;

    /// Plays or resumes playing the sound.
    virtual void
    Play() noexcept = 0;

    /// Stops playing the sound immediately.
    virtual void
    Stop() noexcept = 0;

    /// Applies 3D positioning to the sound.
    virtual void
    Apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept = 0;

    /// Returns true if the audio clip is looping, false otherwise.
    [[nodiscard]] virtual bool
    IsLooped() const noexcept = 0;

    /// Stops looping the sound when it reaches the end of the sound.
    virtual void
    ExitLoop() noexcept = 0;

    /// Gets the current state of the audio source.
    [[nodiscard]] virtual SoundState
    GetState() const noexcept = 0;

    /// Gets the pitch of the audio source.
    [[nodiscard]] virtual float
    GetPitch() const noexcept = 0;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    virtual void
    SetPitch(float pitch) noexcept = 0;

    /// Gets the volume of the audio source.
    [[nodiscard]] virtual float
    GetVolume() const noexcept = 0;

    /// Sets the volume of the audio source (0.0 to 1.0).
    virtual void
    SetVolume(float volume) noexcept = 0;
};

} // namespace Pomdog
