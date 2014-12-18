//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_VOXMODEL_536F1B05_A4BD_4174_8CD3_B1112DBDD7EE_HPP
#define POMDOG_VOXMODEL_536F1B05_A4BD_4174_8CD3_B1112DBDD7EE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

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

#endif // !defined(POMDOG_VOXMODEL_536F1B05_A4BD_4174_8CD3_B1112DBDD7EE_HPP)
