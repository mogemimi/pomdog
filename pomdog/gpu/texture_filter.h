// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"

namespace pomdog::gpu {

enum class TextureFilter : u8 {
    Linear,

    Point,

    Anisotropic,

    LinearMipPoint,

    PointMipLinear,

    MinLinearMagPointMipLinear,

    MinLinearMagPointMipPoint,

    MinPointMagLinearMipLinear,

    MinPointMagLinearMipPoint,
};

} // namespace pomdog::gpu
