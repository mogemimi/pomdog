// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREFILTER_129A8A36_HPP
#define POMDOG_TEXTUREFILTER_129A8A36_HPP

#include <cstdint>

namespace Pomdog {

enum class TextureFilter: std::uint8_t {
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

#endif // POMDOG_TEXTUREFILTER_129A8A36_HPP
