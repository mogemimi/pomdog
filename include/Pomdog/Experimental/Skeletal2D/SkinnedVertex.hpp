// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector4.hpp"
#include <array>
#include <cstdint>

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
