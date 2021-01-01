// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

/// Specifies the states of a sound effect.
enum class SoundState : std::uint8_t {
    /// The sound effect is paused.
    Paused,

    /// The sound effect is playing.
    Playing,

    /// The sound effect is stopped.
    Stopped,
};

} // namespace Pomdog
