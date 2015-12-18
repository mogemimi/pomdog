// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_TEXTUREATLASLOADER_642E468B_HPP
#define POMDOG_TEXTUREATLASLOADER_642E468B_HPP

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

#endif // POMDOG_TEXTUREATLASLOADER_642E468B_HPP
