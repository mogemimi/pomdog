// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector4.hpp"
#include <cstdint>
#include <array>

namespace Pomdog {

class alignas(16) SkinnedVertex {
public:
    // {xy__} = position.xy
    // {__zw} = textureCoord.xy
    Vector4 PositionTextureCoord;

    std::array<float, 4> Weights;

    std::array<std::int32_t, 4> Joints;
};

} // namespace Pomdog
