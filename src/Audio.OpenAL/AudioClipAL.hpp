// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesOpenAL.hpp"
#include "Pomdog/Audio/AudioChannels.hpp"
#include "Pomdog/Audio/AudioClip.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>

namespace Pomdog {
class Error;
enum class AudioChannels : std::uint8_t;
} // namespace Pomdog

namespace Pomdog::Detail::OpenAL {

class AudioClipAL final : public AudioClip {
private:
    std::optional<ALuint> buffer;
    std::size_t sizeInBytes = 0;
    std::int32_t sampleRate = 0;
    std::int32_t bitsPerSample = 0;
    AudioChannels channels = AudioChannels::Mono;

public:
    AudioClipAL() noexcept;

    AudioClipAL(const AudioClipAL&) = delete;
    AudioClipAL& operator=(const AudioClipAL&) = delete;

    ~AudioClipAL() noexcept;

    /// Initializes the audio clip.
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(
        const void* data,
        std::size_t sizeInBytes,
        int sampleRate,
        int bitsPerSample,
        AudioChannels channels) noexcept;

    /// Gets the length of the audio clip in seconds.
    [[nodiscard]] Duration
    GetLength() const noexcept override;

    /// Gets the number of samples per second.
    [[nodiscard]] int
    GetSampleRate() const noexcept override;

    /// Gets the number of bits per sample.
    [[nodiscard]] int
    GetBitsPerSample() const noexcept override;

    /// Gets the number of channels in the audip clip.
    [[nodiscard]] AudioChannels
    GetChannels() const noexcept override;

    /// Gets the size of the audio buffer in bytes.
    [[nodiscard]] std::size_t
    GetSizeInBytes() const noexcept;

    /// Gets the handle of the native OpenAL buffer.
    [[nodiscard]] ALuint
    GetNativeBuffer() const noexcept;
};

} // namespace Pomdog::Detail::OpenAL
