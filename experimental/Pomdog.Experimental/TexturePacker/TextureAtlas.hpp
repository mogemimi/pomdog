// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREATLAS_DDCCEF0C_HPP
#define POMDOG_TEXTUREATLAS_DDCCEF0C_HPP

#include "Pomdog.Experimental/Graphics/TextureRegion.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

namespace Pomdog {
namespace TexturePacker {

struct TextureAtlasPage {
    std::string Name;
};

struct TextureAtlasRegion {
    std::string Name;
    TextureRegion Region;
    std::int16_t TexturePage;
};

class TextureAtlas {
public:
    std::vector<TextureAtlasPage> pages;
    std::vector<TextureAtlasRegion> regions;
};

}// namespace TexturePacker
}// namespace Pomdog

#endif // POMDOG_TEXTUREATLAS_DDCCEF0C_HPP
