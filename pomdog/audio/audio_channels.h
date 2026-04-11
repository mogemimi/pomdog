// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog {

/// Represents the number of channels in the audio clip.
enum class AudioChannels : u8 {
    /// Indicates audio clip contains single channel.
    Mono,

    /// Indicates audio clip contains two channels.
    Stereo,
};

} // namespace pomdog
