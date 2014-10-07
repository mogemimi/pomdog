//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_VOXMODELLOADER_37B4E86A_7006_4BA0_A310_CF2E12FA47CA_HPP
#define POMDOG_VOXMODELLOADER_37B4E86A_7006_4BA0_A310_CF2E12FA47CA_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Pomdog/Content/detail/AssetLoader.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>
#include <array>
#include <cstdint>

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog

namespace Pomdog {
namespace Details {

template <>
struct AssetLoader<MagicaVoxel::VoxModel> {
	MagicaVoxel::VoxModel operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<MagicaVoxel::VoxModel>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_VOXMODELLOADER_37B4E86A_7006_4BA0_A310_CF2E12FA47CA_HPP)
