// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

namespace pomdog {

/// Represents an audio source for playing back an audio clip.
///
/// An AudioSource controls playback of a particular AudioClip, including
/// play, pause, stop, volume, pitch, and looping behavior.
///
/// AudioSources are created via AudioEngine::createAudioSource().
class POMDOG_EXPORT AudioSource {
public:
    AudioSource() noexcept;

    AudioSource(const AudioSource&) = delete;
    AudioSource& operator=(const AudioSource&) = delete;

    virtual ~AudioSource() noexcept;

    /// Pauses the sound.
    virtual void
    pause() noexcept = 0;

    /// Plays or resumes playing the sound.
    virtual void
    play() noexcept = 0;

    /// Stops playing the sound immediately.
    virtual void
    stop() noexcept = 0;

    /// Returns true if the audio source is set to loop, false otherwise.
    [[nodiscard]] virtual bool
    isLooped() const noexcept = 0;

    /// Stops looping the sound when it reaches the end of the sound.
    virtual void
    exitLoop() noexcept = 0;

    /// Returns true if the audio source is currently playing, false otherwise.
    [[nodiscard]] virtual bool
    isPlaying() const noexcept = 0;

    /// Gets the pitch of the audio source.
    /// @return The pitch value in the range [-1.0, 1.0].
    [[nodiscard]] virtual f32
    getPitch() const noexcept = 0;

    /// Sets the pitch of the audio source.
    /// @param pitch The pitch value in the range [-1.0, 1.0].
    ///              -1.0 lowers by one octave, 0.0 is normal, 1.0 raises by one octave.
    virtual void
    setPitch(f32 pitch) noexcept = 0;

    /// Gets the volume of the audio source.
    /// @return The volume value in the range [0.0, 1.0].
    [[nodiscard]] virtual f32
    getVolume() const noexcept = 0;

    /// Sets the volume of the audio source.
    /// @param volume The volume value in the range [0.0, 1.0].
    ///               0.0 is silent, 1.0 is full volume.
    virtual void
    setVolume(f32 volume) noexcept = 0;
};

} // namespace pomdog
