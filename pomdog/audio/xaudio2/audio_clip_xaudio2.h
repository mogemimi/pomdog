// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/xaudio2/prerequisites_xaudio2.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstddef>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
enum class AudioChannels : std::uint8_t;
} // namespace pomdog

namespace pomdog::detail::xaudio2 {

class AudioClipXAudio2 final : public AudioClip {
private:
    std::vector<std::uint8_t> audioData_;
    ::WAVEFORMATEX waveFormat_;
    Duration sampleDuration_ = Duration::zero();
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipXAudio2() noexcept;

    AudioClipXAudio2(const AudioClipXAudio2&) = delete;
    AudioClipXAudio2& operator=(const AudioClipXAudio2&) = delete;

    /// Initializes the audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const void* audioData,
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

    /// Gets the pointer of the WAVEFORMATEX.
    [[nodiscard]] const WAVEFORMATEX*
    getWaveFormat() const noexcept;

    /// Gets the pointer of the audio data.
    [[nodiscard]] const std::uint8_t*
    getData() const noexcept;

    /// Gets the size of the audio data in bytes.
    [[nodiscard]] std::size_t
    getSizeInBytes() const noexcept;
};

} // namespace pomdog::detail::xaudio2
