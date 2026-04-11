// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/openal/openal_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {
class AudioClipFile;
} // namespace pomdog::detail

namespace pomdog::detail::openal {

/// Streaming audio clip implementation for OpenAL.
///
/// Unlike AudioClipAL which loads all audio data into memory,
/// AudioClipStreamingAL reads audio data from a file progressively
/// during playback, reducing memory usage for large audio files.
class AudioClipStreamingAL final : public AudioClip {
private:
    std::unique_ptr<detail::AudioClipFile> audioFile_;
    Duration sampleDuration_ = Duration::zero();

public:
    AudioClipStreamingAL() noexcept;

    AudioClipStreamingAL(const AudioClipStreamingAL&) = delete;
    AudioClipStreamingAL& operator=(const AudioClipStreamingAL&) = delete;

    ~AudioClipStreamingAL() noexcept override;

    /// Initializes the streaming audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(std::unique_ptr<detail::AudioClipFile> audioFile) noexcept;

    /// Gets the length of the audio clip in seconds.
    [[nodiscard]] Duration
    getLength() const noexcept override;

    /// Gets the number of audio samples per channel.
    [[nodiscard]] i32
    getSampleCount() const noexcept override;

    /// Gets the number of samples per second.
    [[nodiscard]] i32
    getSampleRate() const noexcept override;

    /// Gets the number of bits per sample.
    [[nodiscard]] i32
    getBitsPerSample() const noexcept override;

    /// Gets the number of channels in the audio clip.
    [[nodiscard]] AudioChannels
    getChannels() const noexcept override;

    /// Returns true if the audio clip supports streaming.
    ///
    /// `AudioClipStreamingAL` is a streaming audio clip.
    [[nodiscard]] bool
    isStreamable() const noexcept override;

    /// Seeks to the start of the audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    seekStart();

    /// Reads interleaved audio samples into the provided buffer.
    /// @param outBuffer The output buffer to store audio samples.
    /// @param looping Indicates whether to loop the audio when the end is reached.
    /// @return A tuple containing the number of samples read and an error object if an error occurred.
    [[nodiscard]] std::tuple<i32, std::unique_ptr<Error>>
    readSamplesInterleaved(std::span<u8> outBuffer, bool looping);
};

} // namespace pomdog::detail::openal
