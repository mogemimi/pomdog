// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/audio/forward_declarations.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/chrono/duration.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT AudioClip {
public:
    virtual ~AudioClip() noexcept;

    /// Gets the length of the audio clip in seconds.
    [[nodiscard]] virtual Duration
    GetLength() const noexcept = 0;

    /// Gets the number of samples per second.
    [[nodiscard]] virtual int
    GetSampleRate() const noexcept = 0;

    /// Gets the number of bits per sample.
    [[nodiscard]] virtual int
    GetBitsPerSample() const noexcept = 0;

    /// Gets the number of channels in the audip clip.
    [[nodiscard]] virtual AudioChannels
    GetChannels() const noexcept = 0;
};

} // namespace pomdog
