// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Experimental/TexturePacker/TextureAtlas.hpp"
#include <string>

namespace Pomdog {

class AssetManager;

namespace TexturePacker {

class POMDOG_EXPORT TextureAtlasLoader final {
public:
    static TextureAtlas Load(
        const AssetManager& assets,
        const std::string& assetName);
};

} // namespace TexturePacker
} // namespace Pomdog
