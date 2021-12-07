// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/vector4.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class alignas(16) SkinnedVertex final {
public:
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    Vector4 PositionTextureCoord;

    std::array<float, 4> Weights;

    std::array<std::int32_t, 4> Joints;
};

} // namespace Pomdog::Skeletal2D
