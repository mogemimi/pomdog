// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
