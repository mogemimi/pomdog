// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Color.hpp"
#include <array>
#include <cstdint>
#include <vector>

namespace Pomdog {
namespace MagicaVoxel {

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

} // namespace MagicaVoxel
} // namespace Pomdog
