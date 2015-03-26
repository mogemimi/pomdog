// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VOXMODEL_536F1B05_HPP
#define POMDOG_VOXMODEL_536F1B05_HPP

#include "Pomdog/Math/Color.hpp"
#include <vector>
#include <array>
#include <cstdint>

namespace Pomdog {
namespace MagicaVoxel {

class Voxel {
public:
    std::uint8_t X, Y, Z, ColorIndex;
};

class VoxModel {
public:
    std::array<Color, 256> ColorPalette;
    std::vector<Voxel> Voxels;
    std::uint32_t X;
    std::uint32_t Y;
    std::uint32_t Z;
};

}// namespace MagicaVoxel
}// namespace Pomdog

#endif // POMDOG_VOXMODEL_536F1B05_HPP
