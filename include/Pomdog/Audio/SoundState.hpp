// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

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
