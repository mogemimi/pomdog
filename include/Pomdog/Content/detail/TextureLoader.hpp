//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_TEXTURELOADER_17DF87FF_9868_4AE2_A9C4_1319074EC6A3_HPP
#define POMDOG_TEXTURELOADER_17DF87FF_9868_4AE2_A9C4_1319074EC6A3_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../../Config/Export.hpp"
#include "AssetLoader.hpp"

namespace Pomdog {

class Texture2D;

namespace Details {

template <>
struct POMDOG_EXPORT AssetLoader<Texture2D> {
	std::shared_ptr<Texture2D>
	operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<Texture2D>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_TEXTURELOADER_17DF87FF_9868_4AE2_A9C4_1319074EC6A3_HPP)
