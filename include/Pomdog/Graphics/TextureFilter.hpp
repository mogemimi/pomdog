// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>

namespace Pomdog {

enum class TextureFilter : std::uint8_t {
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

} // namespace Pomdog
