// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// TextureAddressMode specifies how texture coordinates outside [0, 1] are handled.
enum class TextureAddressMode : u8 {
    /// Tiles the texture at every integer junction, repeating the pattern.
    Wrap,

    /// Clamps texture coordinates to the range [0, 1], stretching the edge pixels.
    Clamp,

    /// Flips the texture at every integer junction, producing a mirrored repeat.
    Mirror,

    /// Returns the border color for texture coordinates outside [0, 1].
    Border,
};

} // namespace pomdog::gpu
