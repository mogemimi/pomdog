// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT SoundEffect {
public:
    virtual ~SoundEffect() noexcept;

    /// Pauses the sound.
    virtual void
    pause() noexcept = 0;

    /// Plays or resumes playing the sound.
    virtual void
    play() noexcept = 0;

    /// Stops playing the sound immediately.
    virtual void
    stop() noexcept = 0;

    /// Applies 3D positioning to the sound.
    virtual void
    apply3D(const AudioListener& listener, const AudioEmitter& emitter) noexcept = 0;

    /// Returns true if the audio clip is looping, false otherwise.
    [[nodiscard]] virtual bool
    isLooped() const noexcept = 0;

    /// Stops looping the sound when it reaches the end of the sound.
    virtual void
    exitLoop() noexcept = 0;

    /// Gets the current state of the audio source.
    [[nodiscard]] virtual SoundState
    getState() const noexcept = 0;

    /// Gets the pitch of the audio source.
    [[nodiscard]] virtual float
    getPitch() const noexcept = 0;

    /// Sets the pitch of the audio source (-1.0 to 1.0).
    virtual void
    setPitch(float pitch) noexcept = 0;

    /// Gets the volume of the audio source.
    [[nodiscard]] virtual float
    getVolume() const noexcept = 0;

    /// Sets the volume of the audio source (0.0 to 1.0).
    virtual void
    setVolume(float volume) noexcept = 0;
};

} // namespace pomdog
