// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Represents the number of channels in the audio clip.
enum class AudioChannels : std::uint8_t {
    /// Indicates audio clip contains single channel.
    Mono = 1,

    /// Indicates audio clip contains two channels.
    Stereo = 2,
};

} // namespace pomdog
