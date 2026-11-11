// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector4.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {

class alignas(16) SkinnedVertex final {
public:
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    Vector4 positionTextureCoord;

    std::array<f32, 4> weights;

    std::array<i32, 4> joints;
};

} // namespace pomdog::skeletal2d
