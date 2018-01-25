// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "VoxModel.hpp"

namespace Pomdog {

class AssetManager;

namespace MagicaVoxel {

struct VoxModelLoader {
    static VoxModel Load(
        AssetManager const& assets,
        std::string const& assetName);
};

} // namespace MagicaVoxel
} // namespace Pomdog
