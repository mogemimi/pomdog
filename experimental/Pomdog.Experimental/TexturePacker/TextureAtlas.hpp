//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP
#define POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "TextureRegion.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <utility>

namespace Pomdog {
namespace Details {
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
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREPACKER_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP)
