//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP
#define POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "SpriteFont.hpp"

namespace Pomdog {
namespace Details {

template <>
struct AssetLoader<SpriteFont> {
	std::shared_ptr<SpriteFont>
	operator()(AssetLoaderContext const& loaderContext,
		std::string const& assetPath);
};

extern template struct AssetLoader<SpriteFont>;

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SPRITEFONTLOADER_30D6FB29_E5E2_407C_B1C9_46B5C3CDE875_HPP)
