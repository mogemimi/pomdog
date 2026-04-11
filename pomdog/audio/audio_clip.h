// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"

namespace pomdog {
enum class AudioChannels : u8;
} // namespace pomdog

namespace pomdog {

/// Represents an audio clip resource.
///
/// An audio clip holds audio data that can be played back via an AudioSource.
/// There are two kinds of audio clips: non-streaming and streaming.
///
/// Non-streaming clips load the entire audio data into memory, while streaming
/// clips read data progressively from a file during playback.
class POMDOG_EXPORT AudioClip {
public:
    AudioClip() noexcept;

    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;

    virtual ~AudioClip() noexcept;

    /// Gets the length of the audio clip in seconds.
    /// For both non-streaming and streaming clips, this returns the exact
    /// total duration computed from the sample count and sample rate.
    [[nodiscard]] virtual Duration
    getLength() const noexcept = 0;

    /// Gets the number of audio samples per channel.
    /// For non-streaming clips, this is the number of decoded samples stored in memory.
    /// For streaming clips, this is the total sample count reported by the file header
    /// (exact for both WAV and Ogg Vorbis).
    [[nodiscard]] virtual i32
    getSampleCount() const noexcept = 0;

    /// Gets the number of samples per second (e.g. 44100, 22050).
    [[nodiscard]] virtual i32
    getSampleRate() const noexcept = 0;

    /// Gets the number of bits per sample (e.g. 8 or 16).
    [[nodiscard]] virtual i32
    getBitsPerSample() const noexcept = 0;

    /// Gets the number of channels in the audio clip (Mono or Stereo).
    [[nodiscard]] virtual AudioChannels
    getChannels() const noexcept = 0;

    /// Returns true if the audio clip uses streaming playback.
    /// Streaming clips decode audio data incrementally during playback
    /// rather than loading the entire file into memory.
    [[nodiscard]] virtual bool
    isStreamable() const noexcept = 0;
};

} // namespace pomdog
