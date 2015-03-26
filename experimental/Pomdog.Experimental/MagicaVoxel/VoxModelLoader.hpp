// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_VOXMODELLOADER_37B4E86A_HPP
#define POMDOG_VOXMODELLOADER_37B4E86A_HPP

#include "VoxModel.hpp"

namespace Pomdog {

class AssetManager;

namespace MagicaVoxel {

class VoxModelLoader final {
public:
    static VoxModel Load(AssetManager const& assets, std::string const& assetName);
};

}// namespace MagicaVoxel
}// namespace Pomdog

#endif // POMDOG_VOXMODELLOADER_37B4E86A_HPP
