//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP
#define POMDOG_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <string>
#include <vector>
#include <Pomdog/Math/Rectangle.hpp>

namespace Pomdog {
namespace Details {
namespace TexturePacker {

struct TextureAtlasPage {
	std::string Name;
};

struct TextureAtlasRegion {
	std::string Name;
	Rectangle Subrect;
	std::uint32_t TexturePage;
	std::uint16_t XOffset;
	std::uint16_t YOffset;
	std::uint16_t OriginalWidth;
	std::uint16_t OriginalHeight;
	bool Rotate;
	bool Flip;
};

class TextureAtlas {
public:
	std::vector<TextureAtlasPage> pages;
	std::vector<TextureAtlasRegion> regions;
};

}// namespace TexturePacker
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREATLAS_DDCCEF0C_6514_4E2B_AD31_7617D2A899B3_HPP)
