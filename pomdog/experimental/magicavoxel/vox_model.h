// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <cstdint>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::magicavoxel {

struct Voxel final {
    std::uint8_t X;
    std::uint8_t Y;
    std::uint8_t Z;
    std::uint8_t ColorIndex;
};

class VoxModel final {
public:
    std::array<Color, 256> ColorPalette;
    std::vector<Voxel> Voxels;
    std::int32_t X;
    std::int32_t Y;
    std::int32_t Z;
};

} // namespace pomdog::magicavoxel
