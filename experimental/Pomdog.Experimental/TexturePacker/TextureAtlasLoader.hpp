// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "TextureAtlas.hpp"
#include <string>

namespace Pomdog {

class AssetManager;

namespace TexturePacker {

class TextureAtlasLoader final {
public:
    static TextureAtlas Load(AssetManager const& assets,
        std::string const& assetName);
};

} // namespace TexturePacker
} // namespace Pomdog
