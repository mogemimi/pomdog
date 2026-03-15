// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/audio_channels.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail {

class POMDOG_EXPORT AudioClipFile {
public:
    AudioClipFile() noexcept;
    AudioClipFile(const AudioClipFile&) = delete;
    AudioClipFile& operator=(const AudioClipFile&) = delete;

    virtual ~AudioClipFile();

    /// Gets the length of the audio clip in samples.
    [[nodiscard]] virtual i32
    getSampleCount() const noexcept = 0;

    /// Gets the number of samples per second.
    [[nodiscard]] virtual i32
    getSampleRate() const noexcept = 0;

    /// Gets the number of bits per sample.
    [[nodiscard]] virtual u16
    getBitsPerSample() const noexcept = 0;

    /// Gets the number of channels in the audio clip.
    [[nodiscard]] virtual AudioChannels
    getChannels() const noexcept = 0;

    /// Seeks to the start of the audio data.
    [[nodiscard]] virtual std::unique_ptr<Error>
    seekStart() = 0;

    /// Reads interleaved samples into the output buffer.
    /// @param outBuffer The buffer to fill with audio samples.
    /// @return A tuple of the number of samples read per channel, and an error if one occurred.
    [[nodiscard]] virtual std::tuple<i32, std::unique_ptr<Error>>
    readSamplesInterleaved(std::span<u8> outBuffer) = 0;
};

} // namespace pomdog::detail
