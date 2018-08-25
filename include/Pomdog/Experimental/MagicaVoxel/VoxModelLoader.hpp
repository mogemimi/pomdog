// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/MagicaVoxel/VoxModel.hpp"

namespace Pomdog {

class AssetManager;

namespace MagicaVoxel {

struct POMDOG_EXPORT VoxModelLoader final {
    static VoxModel Load(
        const AssetManager& assets,
        const std::string& assetName);
};

} // namespace MagicaVoxel
} // namespace Pomdog
