// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/vector4.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class alignas(16) SkinnedVertex final {
public:
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    Vector4 PositionTextureCoord;

    std::array<float, 4> Weights;

    std::array<std::int32_t, 4> Joints;
};

} // namespace pomdog::skeletal2d
