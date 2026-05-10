// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

/// TextureFilter specifies how texels are sampled and filtered during texture minification and magnification.
enum class TextureFilter : u8 {
    /// Bilinear filtering for minification and magnification; linear interpolation between mip levels.
    Linear,

    /// Nearest-neighbor filtering for minification and magnification; nearest mip level selected.
    Point,

    /// Anisotropic filtering; quality scales with `SamplerDesc::maxAnisotropy`.
    Anisotropic,

    /// Bilinear filtering for minification and magnification; nearest mip level selected.
    LinearMipPoint,

    /// Nearest-neighbor filtering for minification and magnification; linear interpolation between mip levels.
    PointMipLinear,

    /// Linear minification, nearest-neighbor magnification, linear mip interpolation.
    MinLinearMagPointMipLinear,

    /// Linear minification, nearest-neighbor magnification, nearest mip level.
    MinLinearMagPointMipPoint,

    /// Nearest-neighbor minification, linear magnification, linear mip interpolation.
    MinPointMagLinearMipLinear,

    /// Nearest-neighbor minification, linear magnification, nearest mip level.
    MinPointMagLinearMipPoint,
};

} // namespace pomdog::gpu
