//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTUREPACKER_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP
#define POMDOG_TEXTUREPACKER_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

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

}// namespace TexturePacker
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREPACKER_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP)
