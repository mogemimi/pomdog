// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/openal/openal_headers.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <span>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::openal {

class AudioClipAL final : public AudioClip {
private:
    std::optional<ALuint> buffer_;
    Duration sampleDuration_ = Duration::zero();
    i32 sampleCount_ = 0;
    i32 samplesPerSec_ = 0;
    i32 bitsPerSample_ = 0;
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipAL() noexcept;

    AudioClipAL(const AudioClipAL&) = delete;
    AudioClipAL& operator=(const AudioClipAL&) = delete;

    ~AudioClipAL() noexcept override;

    /// Initializes the audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::span<const u8> audioData,
        i32 sampleRate,
        i32 bitsPerSample,
        AudioChannels channels) noexcept;

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
    [[nodiscard]] bool
    isStreamable() const noexcept override;

    /// Gets the handle of the native OpenAL buffer.
    [[nodiscard]] ALuint
    getNativeBuffer() const noexcept;
};

} // namespace pomdog::detail::openal
