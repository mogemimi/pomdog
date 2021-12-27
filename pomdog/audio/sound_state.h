// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Specifies the states of a sound effect.
enum class SoundState : std::uint8_t {
    /// The sound effect is paused.
    Paused,

    /// The sound effect is playing.
    Playing,

    /// The sound effect is stopped.
    Stopped,
};

} // namespace pomdog
