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
    std::vector<std::uint8_t> audioData;
    ::WAVEFORMATEX waveFormat;
    Duration sampleDuration = Duration::zero();
    AudioChannels channels = AudioChannels::Mono;

public:
    AudioClipXAudio2() noexcept;

    AudioClipXAudio2(const AudioClipXAudio2&) = delete;
    AudioClipXAudio2& operator=(const AudioClipXAudio2&) = delete;

    /// Initializes the audio clip.
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const void* audioData,
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

    /// Gets the pointer of the WAVEFORMATEX.
    [[nodiscard]] const WAVEFORMATEX*
    GetWaveFormat() const noexcept;

    /// Gets the pointer of the audio data.
    [[nodiscard]] const std::uint8_t*
    GetData() const noexcept;

    /// Gets the size of the audio data in bytes.
    [[nodiscard]] std::size_t
    GetSizeInBytes() const noexcept;
};

} // namespace pomdog::detail::xaudio2
