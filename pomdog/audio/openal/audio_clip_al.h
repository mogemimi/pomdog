// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/openal/prerequisites_openal.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
enum class AudioChannels : std::uint8_t;
} // namespace pomdog

namespace pomdog::detail::openal {

class AudioClipAL final : public AudioClip {
private:
    std::optional<ALuint> buffer_;
    std::size_t sizeInBytes_ = 0;
    std::int32_t sampleRate_ = 0;
    std::int32_t bitsPerSample_ = 0;
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipAL() noexcept;

    AudioClipAL(const AudioClipAL&) = delete;
    AudioClipAL& operator=(const AudioClipAL&) = delete;

    ~AudioClipAL() noexcept override;

    /// Initializes the audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const void* data,
        std::size_t sizeInBytes,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels) noexcept;

    /// Gets the length of the audio clip in seconds.
    [[nodiscard]] Duration
    getLength() const noexcept override;

    /// Gets the number of samples per second.
    [[nodiscard]] int
    getSampleRate() const noexcept override;

    /// Gets the number of bits per sample.
    [[nodiscard]] int
    getBitsPerSample() const noexcept override;

    /// Gets the number of channels in the audip clip.
    [[nodiscard]] AudioChannels
    getChannels() const noexcept override;

    /// Gets the size of the audio buffer in bytes.
    [[nodiscard]] std::size_t
    getSizeInBytes() const noexcept;

    /// Gets the handle of the native OpenAL buffer.
    [[nodiscard]] ALuint
    getNativeBuffer() const noexcept;
};

} // namespace pomdog::detail::openal
