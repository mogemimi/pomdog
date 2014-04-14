//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP
#define POMDOG_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "TextureAtlas.hpp"
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {
namespace Details {

template <>
struct AssetLoader<TexturePacker::TextureAtlas> {
	TexturePacker::TextureAtlas
	operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<TexturePacker::TextureAtlas>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTUREATLASLOADER_642E468B_C154_4E45_BFA9_6C1B437053B5_HPP)
