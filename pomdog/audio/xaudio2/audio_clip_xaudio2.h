// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/audio/audio_clip.h"
#include "pomdog/audio/xaudio2/xaudio2_headers.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::xaudio2 {

class AudioClipXAudio2 final : public AudioClip {
private:
    std::vector<u8> audioData_;
    Duration sampleDuration_ = Duration::zero();
    i32 sizeInBytes_ = 0;
    i32 sampleCount_ = 0;
    i32 samplesPerSec_ = 0;
    i32 bitsPerSample_ = 0;
    AudioChannels channels_ = AudioChannels::Mono;

public:
    AudioClipXAudio2() noexcept;

    AudioClipXAudio2(const AudioClipXAudio2&) = delete;
    AudioClipXAudio2& operator=(const AudioClipXAudio2&) = delete;

    ~AudioClipXAudio2() noexcept override;

    /// Initializes the audio clip with a copy of audioData.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::span<const u8> audioData,
        i32 sampleRate,
        i32 bitsPerSample,
        AudioChannels channels) noexcept;

    /// Initializes the audio clip, taking ownership of audioData.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::vector<u8>&& audioData,
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

    /// Gets the pointer of the audio data.
    [[nodiscard]] unsafe_ptr<const u8>
    getData() const noexcept;

    /// Gets the size of the audio data in bytes.
    [[nodiscard]] i32
    getSizeInBytes() const noexcept;
};

} // namespace pomdog::detail::xaudio2
