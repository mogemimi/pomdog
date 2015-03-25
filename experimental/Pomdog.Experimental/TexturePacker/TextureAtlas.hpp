// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP
#define POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP

#if _MSC_VER > 1000
#pragma once
#endif

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
    std::uint16_t TexturePage;
};

class TextureAtlas {
public:
    std::vector<TextureAtlasPage> pages;
    std::vector<TextureAtlasRegion> regions;
};

}// namespace TexturePacker
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP)
